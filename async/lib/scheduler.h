#pragma once

#include <work.h>
#include <liburing.h>
#include <deque>

#include "event-loop.h"

namespace io {
    template<typename T>
    inline static void closure_invoke(T &&closure) {
        closure.fn(closure.data);
    }




    class scheduler;

    class scheduler_worker {
    public:
        scheduler_worker(const worker_context &ctx, scheduler &scheduler);

        virtual void loop();

    protected:
        virtual bool tick() = 0;

        const worker_context &m_ctx;
        scheduler &m_scheduler;
    };

    class scheduler_request_worker : public scheduler_worker {
    public:
        scheduler_request_worker(const worker_context &ctx, scheduler &scheduler);

        ~scheduler_request_worker();

        void request(request_closure_t closure, task *sourceCtx);

        static scheduler_request_worker *global();

    protected:
        bool tick() override;

    private:
        io_uring m_io_uring{};
        bool m_submit_request{};
        static scheduler_request_worker *g_global_request_worker;
    };

    class scheduler_event_worker : public scheduler_worker {
    public:
        scheduler_event_worker(const worker_context &ctx, scheduler &scheduler);

        ~scheduler_event_worker();

        void queue_for_cleanup(task *task_ref);

        void queue_microtask(closure_t closure, closure_t destructor);

        void queue(task *task);

        static scheduler_event_worker *global();

        void request(request_closure_t closure);

        void loop() override;

        static void activate_task();

        [[nodiscard]] int32_t error() const;

        void activate_with_errno(task *task, int32_t error);

        void suspend();

        [[nodiscard]] task *current_task() const;

    protected:
        bool tick() override;

    private:
        std::deque<task *> m_contexts{};
        std::deque<task *> m_cleanup{};
        cothread_t m_thread{};

        void run_loop();

        task *m_active_context{};

        static void io_event_loop();

        uint32_t m_active{};
        std::vector<microtask_t> m_microtasks{};
        int32_t m_errno{};

        thread_local static scheduler_event_worker *g_global_event_worker;
    };

    class scheduler {
    public:
        scheduler();

        ~scheduler();

        void submit_request(std::function<void()> &&request) const;

        void submit_work(std::function<void()> &&work) const;

        void join() const;

        [[nodiscard]] bool has_exited() const;

        void dispatch_exit();

    private:
        void worker_loop(worker_context ctx);

        void request_loop(worker_context ctx);

        worker_pool *m_pool;
        bool m_exit{false};
    };
}