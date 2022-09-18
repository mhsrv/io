#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include "../context/x86.h"

typedef struct stack {
    char *sp;
    size_t stack_length;
} stack_t;

typedef void context_fn(void *test);

stack prepare_stack(char *mem, size_t stack_length) {
    stack stack{mem, stack_length};
    stack.sp += stack.stack_length;
    stack.sp = (char*)((uintptr_t)stack.sp & -16L);
    stack.sp -= 128;
    return stack;
}

void link_context(native_context_t *base, native_context_t *link) {
    base->rbp = link;
    *(void**)base->rsp = (void*)link_handler;
}

native_context_t make_context(stack_t stack, context_fn *fn, void *data) {
    native_context_t context = {0};
    context.rip = (void*)fn;
    context.rsp = (void*)stack.sp;
    context.rdi = data;
    return context;
}

void foo(void *test) {
    printf("you called foo %zu\n", (size_t)test);
    exit(1);
}
void test(void*) {
    printf("first\n");
}

int main() {
    char data[4096];
    char data2[4096];
    auto foo_ctx = make_context(prepare_stack(data, sizeof data), foo, reinterpret_cast<void *>(1234));
    auto test_ctx = make_context(prepare_stack(data2, sizeof data2), test, nullptr);

    link_context(&test_ctx, &foo_ctx);
    set_context(&test_ctx);
}