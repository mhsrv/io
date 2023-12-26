#include <work.h>

io::worker_pool::worker_pool(const std::function<void(worker_context)>& workerInit,
                             const std::function<void(worker_context)>& requestInit) {
    for(size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
        if (i == 0) {
            m_threads.push_back(new std::thread([requestInit, this]() {
                requestInit({m_request_channel, *this});
            }));
        }
        else
        {
            m_threads.push_back(new std::thread([workerInit, this]() {
                workerInit({m_worker_channel, *this});
            }));
        }
    }
}

void io::worker_pool::submit_request(std::function<void()> &&request) {
    m_request_channel.push(std::move(request));
}

void io::worker_pool::submit_work(std::function<void()> &&work) {
    m_worker_channel.push(std::move(work));
}

void io::worker_pool::join() {
    for(auto& thread : m_threads) {
        thread->join();
        delete thread;
    }
    m_worker_channel.destroy();
    m_request_channel.destroy();
}

bool io::consume_action(const io::worker_context &ctx, size_t max_fails) {
    return consume_action(ctx.channel, max_fails);
}

bool io::consume_action(io::action_channel_t &channel, size_t max_fails) {
    std::function<void()> *action;
    auto fails = 0;
    while(!channel.try_pop(action)) {
        fails++;
        if (fails > max_fails) {
            return false;
        }
        std::this_thread::yield();
    }
    action->operator()();
    delete action;
    return true;
}
