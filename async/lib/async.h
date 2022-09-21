#ifndef IO_ASYNC_H
#define IO_ASYNC_H
#include <event-loop.h>
#include <liburing.h>
#define ASYNC_DEFAULT_STACK_SIZE 4096

#ifdef __cplusplus
extern "C" {
#endif

inline static void async_request(sqe_delegate_fn *fn, void *data) {
    io_event_scheduler_request((scheduler_t*)io_event_scheduler_request_resource(RES_SCHEDULER), (request_closure_t) {
            .fn = fn,
            .data = data
    });
}

inline static task_t *async_defer(delegate_fn *fn, void *data, size_t stack_size) {
    task_t *task = io_task_create((closure_t) {
            .fn = fn,
            .data = data
    }, stack_size);
    io_event_scheduler_queue((scheduler_t*)io_event_scheduler_request_resource(RES_SCHEDULER), task);
    return task;
}

inline static void async_init() {
    io_event_scheduler_thread_activate();
}

inline static void async_await(task_t *task) {
    io_task_await(task);
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <functional>

namespace async {
    inline static void await(task_t* task) {
        async_await(task);
    }

    inline static void init() {
        async_init();
    }

    template<typename T>
    inline static task *defer(void (*fn)(T *), T *data = nullptr, size_t stack_size = ASYNC_DEFAULT_STACK_SIZE) {
        return async_defer(reinterpret_cast<void (*)(void *)>(fn), data, stack_size);
    }

    task *defer(std::function<void()>&& fn);

    template<typename T>
    inline static void request(void (*fn)(T *, io_uring_sqe *), T *data = nullptr) {
        async_request(reinterpret_cast<void (*)(void *, void *)>(fn), data);
    }

    int32_t request(std::function<void (io_uring_sqe*)> fn);


}
#endif
#endif //IO_ASYNC_H
