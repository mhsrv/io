#include <signal.h>
#include "inc/posix.h"

void get_context(native_context_t *c) {
    getcontext(c);
}
void set_context(native_context_t *c) {
    setcontext(c);
}
void swap_context(native_context_t *a, native_context_t *b) {
    swapcontext(a, b);
}
context_stack_t prepare_stack(char *mem, size_t stack_length) {
    context_stack_t stack = {
            .sp = mem,
            .stack_length = stack_length
    };
    return stack;
}

native_context_t make_context(context_stack_t stack, context_fn *fn, void *data, native_context_t* link) {
    native_context_t ctx = {0};
    getcontext(&ctx);
    ctx.uc_stack.ss_size = stack.stack_length;
    ctx.uc_stack.ss_sp = stack.sp;
    ctx.uc_link = link;
    sigemptyset(&ctx.uc_sigmask);
    makecontext(&ctx, (void (*)(void)) fn, 1, data);
    return ctx;
}