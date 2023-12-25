#include <event-loop.h>
#include <thread>
#include <work.h>

thread_local scheduler g_scheduler{};
io::worker_pool pool{};
bool pool_started = false;

scheduler *scheduler_local() {
    return &g_scheduler;
}


template<typename T>
inline static void closure_invoke(T&& closure) {
    closure.fn(closure.data);
}

void io_event_loop() {
    g_scheduler.run_loop();
}

void io_event_scheduler_thread_activate(void) {
    g_scheduler.m_thread = co_create(4096, io_event_loop);
    co_switch(g_scheduler.m_thread);
}

void io_event_scheduler_queue(task_t *task) {
    g_scheduler.queue(task, 0);
}

void io_event_scheduler_request(request_closure_t closure) {
    g_scheduler.request(closure);
}

task_t *io_task_create(closure_t closure, size_t stack_size) {
    return new task {closure, stack_size};
}

thread_local task *g_activating_task = nullptr;

void activate_task() {
    g_scheduler.m_active++;
    auto task_ref = g_activating_task;
    closure_invoke(task_ref->m_closure);
    g_scheduler.queue_for_cleanup(task_ref);
    co_switch(g_scheduler.m_thread);
}

void io_task_await(task_t *task) {
    task->m_callbacks.push_back(g_scheduler.m_active_context);
    co_switch(g_scheduler.m_thread);
}

void io_task_register_destructor(task_t *task, closure_t destructor) {
    task->m_cleanup.push_back(destructor);
}

void io_event_scheduler_queue_microtask(closure_t closure, closure_t destructor) {
    g_scheduler.queue_microtask(closure, destructor);
}

void io_task_suspend() {
    co_switch(g_scheduler.m_thread);
}

void task::activate() {
    if (!m_active) {
        m_active = true;
        m_thread = co_create(m_size, activate_task);
        g_activating_task = this;
        co_switch(m_thread);
    } else {
        co_switch(m_thread);
    }
}


task::task(closure_t closure, size_t stack_size)
        : m_closure(closure), m_size(stack_size) {

}

task::~task() {
    co_delete(m_thread);
}

void scheduler::queue(task *task, int32_t error) {
    m_contexts.emplace_back(task, error);
}

io_uring g_io_ring{};

__attribute__((constructor)) void init_uring() {
    pool.soft_start();
    struct io_uring_params params{};
//    params.flags |= IORING_SETUP_SQPOLL;
//    params.features |= IORING_FEAT_SQPOLL_NONFIXED;
//    params.sq_thread_idle = IO_URING_SQ_THREAD_IDLE;
    io_uring_queue_init_params(IO_URING_DEPTH, &g_io_ring, &params);


}

__attribute__((destructor())) void destroy_uring() {
    io_uring_queue_exit(&g_io_ring);

}

bool g_submit_requests = false;


void scheduler::request(request_closure_t closure) const {
    auto ctx = m_active_context;
    pool.execute_on_main_thread([closure, ctx]{
        io_uring_sqe *sqe;
        while((sqe = io_uring_get_sqe(&g_io_ring)) == nullptr) {}
        io_uring_sqe_set_data(sqe, ctx);
        closure.fn(closure.data, sqe);
        g_submit_requests = true;
    });
    co_switch(g_scheduler.m_thread);
}


void io_event_scheduler_main_thread() {
    auto activateWithErrno = [](task *task, int32_t err){
        if (!pool_started) {
            g_scheduler.queue(task, err);
        } else {
            pool.queue_work([task, err](io::worker_context ctx) {
                g_scheduler.queue(task, err);
            });
        }
    };
    auto process_single_cqe = [&activateWithErrno](io_uring_cqe *cqe) {
        auto *ctx = reinterpret_cast<task *>(io_uring_cqe_get_data(cqe));
        io_uring_cqe_seen(&g_io_ring, cqe);
        activateWithErrno(ctx, -cqe->res);
    };

    auto process_cqe = [&process_single_cqe]{
        io_uring_cqe *cqe{};

        while (io_uring_peek_cqe(&g_io_ring, &cqe) == 0) {
            process_single_cqe(cqe);
        }

    };

    auto submit_requests = [] {
        if (g_submit_requests) {
            g_submit_requests = false;
            io_uring_submit(&g_io_ring);
        }
    };

    process_cqe();
    submit_requests();
}

void scheduler::run_loop() {
    auto activate = [this](task *ctx, int32_t error) {
        m_active_context = ctx;
        m_errno = error;
        ctx->activate();
        m_errno = 0;
        m_active_context = nullptr;
    };

    auto process_contexts = [this, &activate]{
        if (!m_contexts.empty()){
            auto [ctx, error] = m_contexts.front();
            m_contexts.pop_front();
            activate(ctx, error);
        }
    };

    auto process_microtasks = [this] {
        if (!m_microtasks.empty()) {
            for(const auto & microtask : m_microtasks) {
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
                g_scheduler.queue(callback, 0);
            }

            for (auto& destructor : task_ref->m_cleanup) {
                closure_invoke(destructor);
            }

            m_active--;
            delete task_ref;
        }
    };

    auto is_active = [this] {
        if (pool_started) {
            return io::worker_pool::current().active();
        }
        return m_active || !m_contexts.empty();
    };


    while (is_active()) {
        if (pool_started) {
            io::worker_pool::current().execute();
        } else {
            io::worker_pool::current().execute_main_thread();
            io_event_scheduler_main_thread();
        }
        process_contexts();
        process_microtasks();
        cleanup_contexts();

    }

    pthread_exit(nullptr);
}

void scheduler::queue_for_cleanup(task *task_ref) {
    m_cleanup.push_back(task_ref);
}

void scheduler::queue_microtask(closure_t closure, closure_t destructor) {
    m_microtasks.push_back({closure, destructor});
}


void pool_start() {
    if (pool_started) {
        return;
    }
    pool_started = true;
    pool.start(io_event_scheduler_thread_activate, io_event_scheduler_main_thread);
}
void start_and_queue(const std::function<void()>& fn) {
#if defined(IO_EVENT_LOOP_USE_WORKERS)
    if (!pool_started) {
        pool.queue_work([fn](io::worker_context ctx) {
            fn();
        });
        pool_start();
    }
#else
    fn();
#endif


}