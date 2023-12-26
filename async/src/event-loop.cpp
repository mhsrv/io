#include "event-loop.h"
#include "scheduler.h"

io::scheduler *g_sched = nullptr;

extern "C" {
void io_event_scheduler_start(void) {
    g_sched = new io::scheduler();
}

void io_event_scheduler_join(void) {
    g_sched->join();
    delete g_sched;
    g_sched = nullptr;
}
void io_event_scheduler_queue(task_t *task) {
    g_sched->submit_work([task]{
        io::scheduler_event_worker::global()->queue(task);
    });
}

void io_event_scheduler_request(request_closure_t closure) {
    io::scheduler_event_worker::global()->request(closure);
}

void io_event_scheduler_queue_microtask(closure_t closure, closure_t destructor) {
    g_sched->submit_work([closure, destructor]{
        io::scheduler_event_worker::global()->queue_microtask(closure, destructor);
    });
}

void io_task_suspend() {
    io::scheduler_event_worker::global()->suspend();
}

void io_task_register_destructor(task_t *task, closure_t destructor) {
    task->m_cleanup.push_back(destructor);
}


task_t *io_task_current() {
    return io::scheduler_event_worker::global()->current_task();
}


void io_task_await(task_t *task) {
    task->m_callbacks.push_back(io_task_current());
    io_task_suspend();

}
task_t *io_task_create(closure_t closure, size_t stack_size) {
    return new io::task {closure, stack_size};
}

}

int io_get_errno() {
    return io::scheduler_event_worker::global()->error();
}
