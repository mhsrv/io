#pragma once

#include <rigtorp/MPMCQueue.h>

#include <functional>
#include <thread>
#include <vector>

namespace io {
    class worker_pool;

    class worker_context {
    public:
        constexpr worker_context(uint64_t thread_id, bool *terminate, worker_pool &pool);
        void terminate() const;
        void execute();
        void execute_main_thread();
        [[nodiscard]] bool active() const;
        [[nodiscard]] worker_pool &pool() const;
        [[nodiscard]] uint64_t id() const;

    private:
        const uint64_t m_thread_id;
        worker_pool &m_pool;
        bool *const m_terminate;
    };

    class worker_pool {
    public:
        void execute_on_main_thread(std::function<void()> &&fn);
        void queue_work(std::function<void(worker_context &ctx)> &&fn);
        void start(void init(), void main_tick());
        void stop();
        void soft_start();
        static worker_context& current();
        rigtorp::MPMCQueue<std::function<void(worker_context &ctx)>*> m_workload{1024};
        rigtorp::MPMCQueue<std::function<void()>*> m_main_thread_workload{1024};

    private:
        void init_loop(uint64_t thread_id);
        static thread_local bool m_should_terminate;
    };
}