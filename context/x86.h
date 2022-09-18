#ifndef M_X86_CONTEXTS
#ifdef __cplusplus
#pragma once
extern "C"
{
#endif

typedef struct native_context {
    void *rip, *rsp;
    void *rbx, *rbp, *r12, *r13, *r14, *r15, *rdi;
} native_context_t;

extern void get_context(native_context_t *c);
extern void set_context(native_context_t *c);
extern void swap_context(native_context_t *a, native_context_t *b);
extern void link_handler(void);
#ifdef __cplusplus
};
#endif
#endif
#define M_X86_CONTEXTS