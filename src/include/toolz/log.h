#ifndef M_LOG_H
#define M_LOG_H

#include <stdio.h>
#include <stdarg.h>

#define L_ERROR 1
#define L_WARN 2
#define L_DEBUG 3
#define L_DEBUG12 12

inline void log_(int level, void *arg, const char *format, ...){
    va_list va;
    va_start(va, format);
    vfprintf(stderr, format, va);
    va_end(va);
}

#define LOG(args...) log_(args)

inline void _M_LOG(int level, const char *format, ...){
    va_list va;
    va_start(va, format);
    vfprintf(stderr, format, va);
    va_end(va);
}

#endif
