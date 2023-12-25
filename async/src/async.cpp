#include "async.h"

void request_handler(std::function<void(io_uring_sqe *)> *fn, io_uring_sqe *sqe) {
    fn->operator()(sqe);
}

int32_t async::request(std::function<void(io_uring_sqe *)> fn) {
    request(request_handler, &fn);
    return scheduler_local()->m_errno;
}

void closure_handler(void *data) {
    reinterpret_cast<std::function<void()>*>(data)->operator()();
}

void closure_cleanup(void *fn_ref) {
    delete reinterpret_cast<std::function<void()> *>(fn_ref);
}

void defer_handler(std::function<void()> *fn) {
    closure_handler(fn);
}

task *async::defer(std::function<void()> &&fn) {
    auto* callback = new std::function(fn);
    auto* task = defer(defer_handler, callback);
    io_task_register_destructor(task, {closure_cleanup, callback});
    return task;
}

void async::queue_microtask(std::function<void()> &&closure) {
    auto* callback = new std::function(closure);
    async_queue_microtask({.fn = closure_handler, .data = callback}, {.fn = closure_cleanup, .data = callback});
}

void async::init(std::function<void()> &&fn) {
    auto* callback = new std::function(fn);
    start_and_queue([callback]{
        auto* task = defer(defer_handler, callback);
        io_task_register_destructor(task, {closure_cleanup, callback});
    });
    async_init();
}