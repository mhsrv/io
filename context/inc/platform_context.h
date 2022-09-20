#ifndef IO_PLATFORM_CONTEXT_H
#define IO_PLATFORM_CONTEXT_H

#if defined(__x86_64) && !defined(IO_POSIX)
#include "x86.h"
#else
#include "posix.h"
#endif

#endif //IO_PLATFORM_CONTEXT_H
