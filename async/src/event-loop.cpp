#include <event-loop.h>
#include <thread>

thread_local scheduler g_scheduler{};

template<typename T>
inline static void closure_invoke(T&& closure) {
    closure.fn(closure.data);
}

void io_event_loop(void) {
    g_scheduler.run_loop();
}

void io_event_scheduler_thread_activate(void) {
    g_scheduler.m_thread = co_create(4096, io_event_loop);
    co_switch(g_scheduler.m_thread);
}

void io_event_scheduler_queue(scheduler_t *scheduler, task_t *task) {
    scheduler->queue(task);
}

void io_event_scheduler_request(scheduler_t *scheduler, request_closure_t closure) {
    scheduler->request(closure);
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

void *io_event_scheduler_request_resource(scheduler_resource_t resource) {
    switch (resource) {
        case RES_URING:
            return &g_scheduler.m_io_uring;
        case RES_ERRNO:
            return &g_scheduler.m_errno;
        case RES_ACTIVE_CONTEXT:
            return g_scheduler.m_active_context;
        case RES_SCHEDULER:
            return &g_scheduler;
    }
    return nullptr;
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

void scheduler::queue(task *task) {
    m_contexts.push_back(task);
}

void scheduler::request(request_closure_t closure) {
    io_uring_sqe *sqe;
    while((sqe = io_uring_get_sqe(&m_io_uring)) == nullptr) {
        m_contexts.push_back(g_scheduler.m_active_context);
        co_switch(g_scheduler.m_thread);
    }
    io_uring_sqe_set_data(sqe, m_active_context);
    closure.fn(closure.data, sqe);
    m_submit_request = true;
    co_switch(g_scheduler.m_thread);
}


scheduler::scheduler() {
    struct io_uring_params params{};
#if !defined(IO_EVENT_LOOP_HIGH_EFFICIENCY) || defined(IO_EVENT_LOOP_SQPOLL)
    params.flags |= IORING_SETUP_SQPOLL;
    params.features |= IORING_FEAT_SQPOLL_NONFIXED;
    params.sq_thread_idle = IO_URING_SQ_THREAD_IDLE;
#endif
    io_uring_queue_init_params(IO_URING_DEPTH, &m_io_uring, &params);
}

scheduler::~scheduler() {
    io_uring_queue_exit(&m_io_uring);
}

void scheduler::run_loop() {
    auto activate = [this](task *ctx) {
        m_active_context = ctx;
        ctx->activate();
        m_active_context = nullptr;
    };

    auto process_single_cqe = [this, &activate](io_uring_cqe *cqe) {
        auto *ctx = reinterpret_cast<task *>(io_uring_cqe_get_data(cqe));
        m_errno = -cqe->res;
        io_uring_cqe_seen(&m_io_uring, cqe);
        activate(ctx);
        m_errno = 0;
    };

    auto process_cqe = [this, &process_single_cqe]{
        io_uring_cqe *cqe{};

        while (io_uring_peek_cqe(&m_io_uring, &cqe) == 0) {
            process_single_cqe(cqe);
        }

#if defined(IO_EVENT_LOOP_HIGH_EFFICIENCY) || defined(IO_EVENT_LOOP_WAIT_FOR_CQE)
        if (m_contexts.empty() && m_microtasks.empty() && !m_submit_request) {
            if (io_uring_wait_cqe(&m_io_uring, &cqe) == 0) {
                process_single_cqe(cqe);
            }
        }
#endif
    };

    auto process_contexts = [this, &activate]{
        if (!m_contexts.empty()){
            auto front = m_contexts.front();
            m_contexts.pop_front();
            activate(front);
        }
    };

    auto process_microtasks = [this] {
        if (!m_microtasks.empty()) {
            for(size_t i = 0; i < m_microtasks.size(); i++) {
                const auto& microtask = m_microtasks[i];
                closure_invoke(microtask.closure);
                closure_invoke(microtask.destructor);
            }

            m_microtasks.clear();
        }
    };

    auto submit_requests = [this] {
        if (m_submit_request) {
            m_submit_request = false;
            io_uring_submit(&m_io_uring);
        }
    };

    auto cleanup_contexts = [this] {
        while(!m_cleanup.empty()) {
            auto task_ref = m_cleanup.front();
            m_cleanup.pop_front();

            for (auto& callback : task_ref->m_callbacks) {
                g_scheduler.queue(callback);
            }

            for (auto& destructor : task_ref->m_cleanup) {
                closure_invoke(destructor);
            }

            m_active--;
            delete task_ref;
        }
    };

    auto is_active = [this] {
        return m_active || m_contexts.size();
    };

    while (is_active()) {
        process_cqe();
        process_contexts();
        process_microtasks();
        submit_requests();
        cleanup_contexts();
    }

    pthread_exit(0);
}

void scheduler::queue_for_cleanup(task *task_ref) {
    m_cleanup.push_back(task_ref);
}

void scheduler::queue_microtask(closure_t closure, closure_t destructor) {
    m_microtasks.push_back({closure, destructor});
}
