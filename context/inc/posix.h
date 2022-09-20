#ifndef IO_POSIX_H
#define IO_POSIX_H

#ifdef __cplusplus
#pragma once
extern "C"
{
#endif
#include <ucontext.h>

typedef ucontext_t native_context_t;

#include "common.h"

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //IO_POSIX_H
