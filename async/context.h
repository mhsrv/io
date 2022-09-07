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

    static context self() {
        return context(nullptr);
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
        VALGRIND_STACK_REGISTER((char*)m_ucontext.uc_stack.ss_sp, (char*)m_ucontext.uc_stack.ss_sp + m_ucontext.uc_stack.ss_size);
        makecontext(&m_ucontext, reinterpret_cast<void (*)()>(lambda_context<size>::call_lambda), 1, reinterpret_cast<void*>(fn));
    }
    ~lambda_context() {
        VALGRIND_STACK_DEREGISTER((char*)m_ucontext.uc_stack.ss_sp);
    }
private:
    static void call_lambda(std::function<void()>* f) {
        f->operator()();

    }
    char m_stack[size]{};
};