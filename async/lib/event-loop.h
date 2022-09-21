#ifndef IO_EVENT_LOOP_H
#define IO_EVENT_LOOP_H

#include <libco.h>

#define IO_URING_DEPTH 1024*32
#define IO_URING_SQ_THREAD_IDLE 120000
typedef void delegate_fn(void *data);
typedef void sqe_delegate_fn(void *data, void *sqe);

typedef struct closure {
    delegate_fn *fn;
    void *data;
} closure_t;

typedef struct request_closure {
    sqe_delegate_fn *fn;
    void *data;
} request_closure_t;

#ifdef __cplusplus
#include <cstdlib>
#include <deque>
#include <vector>
#include <cstdint>
#include <liburing.h>
struct task;

typedef struct microtask {
    closure_t closure;
    closure_t destructor;
} microtask_t;

typedef class scheduler {
public:
    scheduler();
    void queue(task *task);
    void request(request_closure_t closure);
    void queue_for_cleanup(task *task_ref);
    void queue_microtask(closure_t closure, closure_t destructor);
    void run_loop();
    ~scheduler();

    cothread_t m_thread;
    std::deque<task*> m_contexts{};
    std::deque<task*> m_cleanup{};
    std::vector<microtask_t> m_microtasks{};
    int32_t m_errno{};
    uint32_t m_active{};
    io_uring m_io_uring{};
    task* m_active_context;
    bool m_submit_request{};
} scheduler_t;


typedef struct task {
    task(closure_t closure, size_t stack_size);
    void activate();
    std::deque<task*> m_callbacks{};
    std::deque<closure> m_cleanup{};
    ~task();
    closure_t m_closure;
    size_t m_size;
private:
    cothread_t m_thread{};
    bool m_active{false};
} task_t;


#else
#include <stdlib.h>
typedef void scheduler_t;
typedef void task_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum scheduler_resource {
    RES_SCHEDULER = 0,
    RES_URING = 1,
    RES_ERRNO = 2,
    RES_ACTIVE_CONTEXT = 3
} scheduler_resource_t;

void io_event_scheduler_thread_activate(void);
void io_event_scheduler_queue(scheduler_t *scheduler, task_t *task);
void io_event_scheduler_request(scheduler_t *scheduler, request_closure_t closure);
void *io_event_scheduler_request_resource(scheduler_resource_t resource);
void io_event_scheduler_queue_microtask(closure_t closure, closure_t destructor);
void io_task_suspend();
void io_task_register_destructor(task_t *task, closure_t destructor);
void io_task_await(task_t *task);
task_t *io_task_create(closure_t closure, size_t stack_size);

#ifdef __cplusplus
}
#endif
#endif //IO_EVENT_LOOP_H
