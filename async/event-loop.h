#pragma once
#include <liburing.h>
#include <deque>
#include "context.h"

struct task_descriptor {
    context *cleaner;
    context *main;
    std::function<void()> *cleaner_fn;
    std::function<void()> *main_fn;
};


#define EVENT_LOOP_DEPTH 32
#define EVENT_LOOP_DEFAULT_TASK_STACK_SIZE 4096
#define EVENT_LOOP_CLEANUP_STACK_SIZE 128

class event_loop {
public:
    static event_loop* self(int32_t event_loop_depth = -1) {
        static event_loop* value = nullptr;
        static int32_t depth = 0;
        if (event_loop_depth == -1) {
            if (depth == 0) {
                event_loop_depth = EVENT_LOOP_DEPTH;
            } else {
                event_loop_depth = depth;
            }
        }

        if (event_loop_depth == depth) {
            return value;
        }

        if (value != nullptr) {
            delete value;
        }

        depth = event_loop_depth;
        value = new event_loop(event_loop_depth);
        return value;
    }
    ~event_loop() {
        io_uring_queue_exit(&m_io_uring);
    }


    [[nodiscard]] int32_t error() const {
        return m_errno;
    }


    void yield() {
        m_contexts.push_back(m_active_context);
        yield_scheduler();
    }

    int32_t request(const std::function<void(io_uring_sqe*)>& request_factory) {
        m_submit_request = true;
        auto *sqe = io_uring_get_sqe(&m_io_uring);
        io_uring_sqe_set_data(sqe, m_active_context);
        request_factory(sqe);
        yield_scheduler();
        return error();
    }

    void start() {
        static auto taskDescriptor = task_descriptor();
        taskDescriptor.main_fn = new std::function([this]{
            this->run();
        });

        taskDescriptor.cleaner_fn = new std::function([&] {
            delete taskDescriptor.main_fn;
            delete taskDescriptor.main;
            delete taskDescriptor.cleaner_fn;
            delete taskDescriptor.cleaner;
        });

        taskDescriptor.cleaner = new lambda_context<EVENT_LOOP_CLEANUP_STACK_SIZE>(taskDescriptor.cleaner_fn, nullptr);
        taskDescriptor.main = new lambda_context<4096>(taskDescriptor.main_fn, taskDescriptor.cleaner);

        static auto context = context::self(taskDescriptor.main);
        m_contexts.push_back(&context);
        context.swap(taskDescriptor.main);
    }

    template<size_t stack_size = EVENT_LOOP_DEFAULT_TASK_STACK_SIZE>
    void queue_task(const std::function<void()>& task) {
        m_contexts.push_back(construct_task<stack_size>(task));
    }

private:
    event_loop(const event_loop&) = delete;
    event_loop& operator= (const event_loop&) = delete;
    event_loop(uint32_t entries) : m_entries(entries), m_scheduler(context::self()) {
        io_uring_queue_init(m_entries, &m_io_uring, 0);
    }

    [[nodiscard]] bool active() const {
        return m_active > 0 || !m_contexts.empty() || m_last_active || m_submit_request;
    }

    void run() {
        while (active()) {
            if (!next()) {
                sched_yield();
            }
        }
    }

    void yield_scheduler() {
        m_active_context->swap(&m_scheduler);
    }

    bool next() {
        if (m_last_active != nullptr) {
            delete m_last_active->main;
            delete m_last_active->cleaner;
            delete m_last_active->main_fn;
            delete m_last_active->cleaner_fn;
            delete m_last_active;
            m_last_active = nullptr;
            return true;
        }

        if (m_submit_request) {
            m_submit_request = false;
            m_active += io_uring_submit(&m_io_uring);
            return true;
        }


        io_uring_cqe *cqe;
        if (io_uring_peek_cqe(&m_io_uring, &cqe) == 0) {
            auto* ctx = reinterpret_cast<context*>(io_uring_cqe_get_data(cqe));
            m_errno = -cqe->res;
            m_active_context = ctx;
            m_scheduler.swap(ctx);
            m_active_context = nullptr;
            m_errno = 0;
            io_uring_cqe_seen(&m_io_uring, cqe);
            m_active--;
            return true;
        }

        if (!m_contexts.empty()) {
            auto front = m_contexts.front();
            m_contexts.pop_front();
            m_active_context = front;
            m_scheduler.swap(front);
            m_active_context = nullptr;
            return true;
        }

        return false;
    }

    template<size_t stack_size>
    lambda_context<stack_size> *construct_task(const std::function<void()>& task) {
        auto* taskDescriptor = new task_descriptor{};
        auto cleanupFn = new std::function([taskDescriptor, this]{
            this->m_last_active = taskDescriptor;
        });
        auto mainFn = new std::function(task);
        auto cleanup = new lambda_context<EVENT_LOOP_CLEANUP_STACK_SIZE>(cleanupFn, &m_scheduler);
        auto main = new lambda_context<stack_size>(mainFn, cleanup);
        taskDescriptor->cleaner = cleanup;
        taskDescriptor->main = main;
        taskDescriptor->cleaner_fn = cleanupFn;
        taskDescriptor->main_fn = mainFn;
        return main;
    }

    io_uring m_io_uring{};
    uint32_t m_entries;
    bool m_submit_request{};
    uint32_t m_active{};
    int32_t m_errno{};
    std::deque<context*> m_contexts{};
    context m_scheduler;
    context *m_active_context{};
    task_descriptor *m_last_active{};
};