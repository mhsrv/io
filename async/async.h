#pragma once

#include "event-loop.h"

namespace async {
    void yield() {
        event_loop::self()->yield();
    }

    template<size_t stack_size = EVENT_LOOP_DEFAULT_TASK_STACK_SIZE>
    task *defer(const std::function<void()>& task) {
        return event_loop::self()->queue_task<stack_size>(task);
    }

    void await(task* value) {
        value->wait();
    }

    template<size_t stack_size = EVENT_LOOP_DEFAULT_TASK_STACK_SIZE>
    void await(const std::function<void()>& task) {
        await(defer<stack_size>(task));
    }

    void init(int32_t event_loop_depth = -1) {
        event_loop::self(event_loop_depth)->start();
    }
}


#define ASYNC_INIT(__DEPTH__) [[gnu::constructor, maybe_unused]] void _async_init() { \
    async::init(__DEPTH__);                                                           \
}                                                                                     \
struct _async {}
