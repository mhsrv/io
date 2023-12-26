#include <scheduler.h>
#include <stdexcept>
void io::scheduler_worker::loop() {
    while (!m_scheduler.has_exited()) {
        if (!consume_action(m_ctx)) {
            std::this_thread::yield();
        }
        if (!tick()) {
            m_scheduler.dispatch_exit();
        }
    }
}

thread_local io::task *g_activating_task = nullptr;

void io::scheduler_event_worker::activate_task() {
    io::scheduler_event_worker::global()->m_active++;
    auto task_ref = g_activating_task;
    closure_invoke(task_ref->m_closure);
    io::scheduler_event_worker::global()->queue_for_cleanup(task_ref);
    co_switch(io::scheduler_event_worker::global()->m_thread);
}

void io::scheduler_event_worker::queue_for_cleanup(task *task_ref) {
    m_cleanup.push_back(task_ref);
}


void io::scheduler_event_worker::queue(task *task) {
    m_contexts.push_back(task);
}

void io::task::activate() {
    if (!m_active) {
        m_active = true;
        m_thread = co_create(m_size, io::scheduler_event_worker::activate_task);
        g_activating_task = this;
        co_switch(m_thread);
    } else {
        co_switch(m_thread);
    }
}


io::task::task(closure_t closure, size_t stack_size)
        : m_closure(closure), m_size(stack_size) {

}

io::task::~task() {
    co_delete(m_thread);
}


io::scheduler_worker::scheduler_worker(const worker_context &ctx, scheduler &scheduler)
        : m_ctx(ctx), m_scheduler(scheduler) {}

thread_local io::scheduler_event_worker *io::scheduler_event_worker::g_global_event_worker = nullptr;
io::scheduler_event_worker *io::scheduler_event_worker::global() {
    return g_global_event_worker;
}

void io::scheduler::request_loop(io::worker_context ctx) {
    (new scheduler_request_worker(ctx, *this))->loop();

}

void io::scheduler::worker_loop(io::worker_context ctx) {
    (new scheduler_event_worker(ctx, *this))->loop();
}

void io::scheduler::dispatch_exit() {
    m_exit = true;
}

bool io::scheduler::has_exited() const {
    return m_exit;
}

void io::scheduler::join() const {
    m_pool->join();
}

io::scheduler::~scheduler() {
    delete m_pool;
}

io::scheduler::scheduler() {
    std::function requestInit([this](io::worker_context ctx) {
        request_loop(ctx);
    });
    std::function workerInit([this](io::worker_context ctx) {
        worker_loop(ctx);
    });
    m_pool = new io::worker_pool(workerInit, requestInit);
}

void io::scheduler::submit_request(std::function<void()> &&request) const {
    m_pool->submit_request(std::move(request));
}

void io::scheduler::submit_work(std::function<void()> &&work) const {
    m_pool->submit_work(std::move(work));
}

class worker_duplicate_init_error: public std::runtime_error {
public:
    worker_duplicate_init_error() : std::runtime_error(message) {}

private:

    static const char *message;
};

const char *worker_duplicate_init_error::message = "Failed initialization of worker for this thread";

io::scheduler_request_worker::scheduler_request_worker(const io::worker_context &ctx, io::scheduler &scheduler)
        : scheduler_worker(ctx, scheduler) {
    if (g_global_request_worker) {
        throw worker_duplicate_init_error();
    }
    struct io_uring_params params{};
#if !defined(IO_EVENT_LOOP_HIGH_EFFICIENCY) || defined(IO_EVENT_LOOP_SQPOLL)
    params.flags |= IORING_SETUP_SQPOLL;
    params.features |= IORING_FEAT_SQPOLL_NONFIXED;
    params.sq_thread_idle = IO_URING_SQ_THREAD_IDLE;
#endif
    io_uring_queue_init_params(IO_URING_DEPTH, &m_io_uring, &params);
    g_global_request_worker = this;
}

io::scheduler_event_worker::scheduler_event_worker(const io::worker_context &ctx, io::scheduler &scheduler)
        : scheduler_worker(ctx, scheduler) {
    if (g_global_event_worker) {
        throw worker_duplicate_init_error();
    }

    g_global_event_worker = this;
}

bool io::scheduler_event_worker::tick() {
    return false;
}

io::scheduler_event_worker::~scheduler_event_worker() {
    g_global_event_worker = nullptr;
}

bool io::scheduler_request_worker::tick() {
    auto process_single_cqe = [this](io_uring_cqe *cqe) {
        auto *ctx = reinterpret_cast<task *>(io_uring_cqe_get_data(cqe));
        auto error = -cqe->res;
        io_uring_cqe_seen(&m_io_uring, cqe);
        m_scheduler.submit_work([ctx, error]{
            scheduler_event_worker::global()->activate_with_errno(ctx, error);
        });
    };

    auto process_cqe = [this, &process_single_cqe]{
        io_uring_cqe *cqe{};

        while (io_uring_peek_cqe(&m_io_uring, &cqe) == 0) {
            process_single_cqe(cqe);
        }
    };


    auto submit_requests = [this] {
        if (m_submit_request) {
            m_submit_request = false;
            io_uring_submit(&m_io_uring);
        }
    };

    process_cqe();
    submit_requests();

    return true;
}

io::scheduler_request_worker::~scheduler_request_worker() {
    io_uring_queue_exit(&m_io_uring);
    g_global_request_worker = nullptr;
}

io::scheduler_request_worker *io::scheduler_request_worker::g_global_request_worker = nullptr;

io::scheduler_request_worker *io::scheduler_request_worker::global() {
    return g_global_request_worker;
}

void io::scheduler_request_worker::request(request_closure_t closure, task* sourceCtx) {
    io_uring_sqe *sqe;
    while((sqe = io_uring_get_sqe(&m_io_uring)) == nullptr) {
        std::this_thread::yield();
    }
    io_uring_sqe_set_data(sqe, sourceCtx);
    closure.fn(closure.data, sqe);
    m_submit_request = true;
}

void io::scheduler_event_worker::io_event_loop() {
    io::scheduler_event_worker::global()->run_loop();
}

void io::scheduler_event_worker::loop() {
    m_thread = co_create(4096*1024, io_event_loop);
    co_switch(m_thread);
}



void io::scheduler_event_worker::queue_microtask(closure_t closure, closure_t destructor) {
    m_microtasks.push_back({closure, destructor});
}

void io::scheduler_event_worker::run_loop() {

    auto process_contexts = [this]{
        if (!m_contexts.empty()) {
            auto front = m_contexts.front();
            m_contexts.pop_front();
            activate_with_errno(front, 0);
        }
    };

    auto process_microtasks = [this] {
        if (!m_microtasks.empty()) {
            for(const auto microtask : m_microtasks) {
                closure_invoke(microtask.closure);
                closure_invoke(microtask.destructor);
            }

            m_microtasks.clear();
        }
    };

    auto cleanup_contexts = [this] {
        while(!m_cleanup.empty()) {
            auto task_ref = m_cleanup.front();
            m_cleanup.pop_front();

            for (auto& callback : task_ref->m_callbacks) {
                queue(callback);
            }

            for (auto& destructor : task_ref->m_cleanup) {
                closure_invoke(destructor);
            }

            m_active--;
            delete task_ref;
        }
    };

    auto is_active = [this] {
        return m_active || !m_contexts.empty();
    };

    while(!m_scheduler.has_exited()) {
        consume_action(m_ctx);

        if (!is_active()) {
            std::this_thread::yield();
            continue;
        }
        process_contexts();
        process_microtasks();
        cleanup_contexts();
    }

    pthread_exit(nullptr);

}

void io::scheduler_event_worker::activate_with_errno(io::task *task, int32_t error) {
    m_errno = error;
    m_active_context = task;
    task->activate();
    m_active_context = nullptr;
    m_errno = 0;
}

void io::scheduler_event_worker::request(request_closure_t closure) {
    m_scheduler.submit_request([closure, ctx = m_active_context]{
        io::scheduler_request_worker::global()->request(closure, ctx);
    });
    co_switch(m_thread);
}

int32_t io::scheduler_event_worker::error() const {
    return m_errno;
}

void io::scheduler_event_worker::suspend() {
    co_switch(m_thread);
}

io::task *io::scheduler_event_worker::current_task() const {
    return m_active_context;
}
