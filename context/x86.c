#include "inc/x86.h"

#include <stdint.h>

extern void link_handler(void);

context_stack_t prepare_stack(char *mem, size_t stack_length) {
    context_stack_t stack = {
            .sp = mem,
            .stack_length = stack_length
    };
    stack.sp += stack.stack_length;
    stack.sp = (char*)((uintptr_t)stack.sp & -16L);
    stack.sp -= 128;
    return stack;
}

native_context_t make_context(context_stack_t stack, context_fn *fn, void *data, native_context_t* link) {
    native_context_t context = {0};
    context.rip = (void*)fn;
    context.rsp = (void*)stack.sp;
    context.rdi = data;
    context.rbp = link;
    *(void**)context.rsp = (void*)link_handler;
    return context;
}
