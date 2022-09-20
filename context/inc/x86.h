#ifndef IO_X86_H
#define IO_X86_H
#ifdef __cplusplus
#pragma once
extern "C"
{
#endif

typedef struct native_context {
    void *rip, *rsp;
    void *rbx, *rbp, *r12, *r13, *r14, *r15, *rdi;
} native_context_t;


#include "common.h"

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //IO_X86_H