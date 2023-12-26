#ifndef IO_EVENT_LOOP_H
#define IO_EVENT_LOOP_H

#include <libco.h>

#ifdef __cplusplus
#include <cstddef>
#include <deque>
#else
#include <stddef.h>
#endif

#define IO_URING_DEPTH (1024*32)
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

typedef struct microtask {
    closure_t closure;
    closure_t destructor;
} microtask_t;

typedef void delegate_fn(void *data);
typedef void sqe_delegate_fn(void *data, void *sqe);

#ifdef __cplusplus
namespace io {
    struct task {
        task(closure_t closure, size_t stack_size);

        void activate();

        std::deque<task *> m_callbacks{};
        std::deque<closure> m_cleanup{};

        ~task();

        closure_t m_closure;
        size_t m_size;
    private:
        cothread_t m_thread;
        bool m_active{false};
    };
}

typedef io::task task_t;

#else
typedef void task_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

void io_event_scheduler_start(void);
void io_event_scheduler_join(void);
void io_event_scheduler_queue(task_t *task);
void io_event_scheduler_request(request_closure_t closure);
void io_event_scheduler_queue_microtask(closure_t closure, closure_t destructor);
void io_task_suspend();
void io_task_register_destructor(task_t *task, closure_t destructor);
void io_task_await(task_t *task);
task_t *io_task_create(closure_t closure, size_t stack_size);
task_t *io_task_current();
int io_get_errno();
#ifdef __cplusplus
}
#endif

#endif //IO_EVENT_LOOP_H
