#include "work.h"

constexpr io::worker_context::worker_context(uint64_t thread_id, bool *terminate, worker_pool &pool)
: m_thread_id(thread_id), m_terminate(terminate), m_pool(pool) {
}

thread_local bool io::worker_pool::m_should_terminate = false;

uint64_t io::worker_context::id() const {
    return m_thread_id;
}

io::worker_pool &io::worker_context::pool() const {
    return m_pool;
}

void io::worker_context::terminate() const {
    *m_terminate = true;
}

void io::worker_context::execute() {
    std::function<void(worker_context &ctx)> *fn = nullptr;
    if (!*m_terminate) {
        if (m_pool.m_workload.try_pop(fn)) {
            if (fn != nullptr) {
                fn->operator()(*this);
                delete fn;
            }
        }
    }
}

bool io::worker_context::active() const {
    return !*m_terminate;
}

void io::worker_context::execute_main_thread() {
    std::function<void()> *fn = nullptr;
    if (!*m_terminate) {
        if (m_pool.m_main_thread_workload.try_pop(fn)) {
            if (fn != nullptr) {
                fn->operator()();
                delete fn;
            }
        }
    }

}

void io::worker_pool::queue_work(std::function<void(worker_context &ctx)> &&fn) {
    auto fn_ptr = new std::function(fn);
    while (!m_workload.try_push(fn_ptr)) { }
}

thread_local io::worker_context* g_ctx = nullptr;



void io::worker_pool::init_loop(uint64_t thread_id) {
    g_ctx = new io::worker_context{thread_id, &m_should_terminate, *this};
}

void io::worker_pool::soft_start() {
    g_ctx = new io::worker_context{0, &m_should_terminate, *this};
}

void io::worker_pool::start(void init(), void main_tick()) {
    auto hardware_threads = std::thread::hardware_concurrency();
    if (hardware_threads > 1) {
        hardware_threads--;
    }
    std::vector<std::thread> threads{};
    for (uint64_t i = 0; i < hardware_threads; i++) {
        threads.push_back(std::move(std::thread([init, this, i] {init_loop(i); init(); delete g_ctx; g_ctx = nullptr; })));
    }
    for (auto &thread: threads) {
        thread.detach();
    }
    while(g_ctx->active()) {
        g_ctx->execute_main_thread();
        main_tick();
    }
    delete g_ctx;
    g_ctx = nullptr;
}


void recursive_exit(io::worker_context &ctx)
{
    if (ctx.id() != 0)
    {
        ctx.terminate();
        ctx.pool().queue_work(recursive_exit);
    }
}


void io::worker_pool::stop() {
    queue_work(recursive_exit);
}

io::worker_context &io::worker_pool::current() {
    return *g_ctx;
}

void io::worker_pool::execute_on_main_thread(std::function<void()> &&fn) {
    auto fn_ptr = new std::function(fn);
    while (!m_main_thread_workload.try_push(fn_ptr)) {
    }
}
