#pragma once

#include "event-loop.h"

namespace async {
    void yield() {
        event_loop::self().yield();
    }

    template<size_t stack_size = EVENT_LOOP_DEFAULT_TASK_STACK_SIZE>
    void queue_task(const std::function<void()>& task) {
        event_loop::self().queue_task<stack_size>(task);
    }

    int start(int main(int argc, char **argv), int argc, char **argv) {
        int ret = 0;
        async::queue_task([main, argc, argv, &ret]{
            ret = main(argc, argv);
        });
        event_loop::self().run();
        return ret;
    }
}

#define ASYNC_ENTRYPOINT(...) int async_main(int, char **); int main(int argc, char **argv) { return async::start(async_main, argc, argv); } int async_main(__VA_ARGS__)