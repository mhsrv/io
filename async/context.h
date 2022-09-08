#pragma once

#include <cstddef>
#include <functional>
#include <ucontext.h>
#include <valgrind/valgrind.h>

class context {
public:
    void swap(const context* context) {
        swapcontext(&m_ucontext, &context->m_ucontext);
    }

    static context self(context *link = nullptr) {
        return context(link);
    }

    static context self_alloc(context *link = nullptr) {
        return new context(link);
    }

    void register_stack() {
        VALGRIND_STACK_REGISTER((char*)m_ucontext.uc_stack.ss_sp, (char*)m_ucontext.uc_stack.ss_sp + m_ucontext.uc_stack.ss_size);
    }

    void unregister_stack() {
        VALGRIND_STACK_DEREGISTER((char*)m_ucontext.uc_stack.ss_sp);
    }

    ucontext_t m_ucontext{};

protected:
    context(context *link) {
        getcontext(&m_ucontext);
        if (link != nullptr) {
            m_ucontext.uc_link = &link->m_ucontext;
        }
    }

};


template<size_t size>
class lambda_context : public context {
public:
    lambda_context(std::function<void()>* fn, context *link = nullptr) : context(link) {
        m_ucontext.uc_stack.ss_size = sizeof m_stack;
        m_ucontext.uc_stack.ss_sp = m_stack;
        sigemptyset(&m_ucontext.uc_sigmask);
        register_stack();
        makecontext(&m_ucontext, reinterpret_cast<void (*)()>(lambda_context<size>::call_lambda), 1, reinterpret_cast<void*>(fn));
    }
    ~lambda_context() {
        unregister_stack();
    }
private:
    static void call_lambda(std::function<void()>* f) {
        f->operator()();

    }
    char m_stack[size]{};
};