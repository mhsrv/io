#include "async.h"

void request_handler(std::function<void(io_uring_sqe *)> *fn, io_uring_sqe *sqe) {
    fn->operator()(sqe);
}

int32_t async::request(std::function<void(io_uring_sqe *)> fn) {
    request(request_handler, &fn);
    return *reinterpret_cast<int32_t*>(io_event_scheduler_request_resource(RES_ERRNO));
}

void defer_handler(std::function<void()> *fn) {
    fn->operator()();
}

void defer_cleanup(void *fn_ref) {
    delete reinterpret_cast<std::function<void()> *>(fn_ref);
}

task *async::defer(std::function<void()> &&fn) {
    auto* callback = new std::function(fn);
    auto* task = defer(defer_handler, callback);
    io_task_register_destructor(task, {defer_cleanup, callback});
    return task;
}
