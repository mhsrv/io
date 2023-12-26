#pragma once

#include <channel.h>
#include <functional>
#include <thread>
#include <vector>

namespace io {
    class worker_pool;

    typedef channel<std::function<void()>> action_channel_t;
    struct worker_context {
        action_channel_t& channel;
        worker_pool& pool;
    };


    class worker_pool {
    public:
        worker_pool(const std::function<void(worker_context ctx)>& workerInit, const std::function<void(worker_context ctx)>& requestInit);
        void submit_request(std::function<void()> &&request);
        void submit_work(std::function<void()> &&work);
        void join();
    private:
        action_channel_t m_request_channel{};
        action_channel_t m_worker_channel{};
        std::vector<std::thread*> m_threads{};
    };

    bool consume_action(const worker_context& ctx, size_t max_fails = 20);
    bool consume_action(action_channel_t& channel, size_t max_fails = 20);
}