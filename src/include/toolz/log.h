#ifndef M_LOG_H
#define M_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define L_ERROR 1
#define L_WARN 2
#define L_DEBUG 3
#define L_DEBUG12 12


struct log_conf_t {
    int log_stderr:1;
    int log_syslog:1;
    int max_log_level;
    const char *subsystem;
    int max_log_subsystem_level;
};

static struct log_conf_t log_conf;

int set_log_conf(log_conf_t *conf);

inline void log_(int level, void *arg, const char *format, ...){
    va_list va;
    va_start(va, format);
    vfprintf(stderr, format, va);
    va_end(va);
}

// #define LOG(args...) log_(args)



void log_main(const char *file, int line, int level, const char *subsystem, const char *format, ...);
#define LOG(level, subsystem, args...)                                           \
    log_main(__FILE__, __LINE__, level, subsystem, args);

inline void _M_LOG(int level, const char *format, ...){
    va_list va;
    va_start(va, format);
    vfprintf(stderr, format, va);
    va_end(va);
}

#endif
