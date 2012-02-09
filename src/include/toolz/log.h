#ifndef M_LOG_H
#define M_LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define L_ERROR 1
#define L_WARN 2
#define L_DEBUG 3
#define L_DEBUG12 12

#ifdef __cplusplus
extern "C" {
#endif



struct log_conf_t {
    int log_stderr:1;
    int log_syslog:1;
    int max_log_level;
    const char *subsystem;
    int max_log_subsystem_level;
};


int set_log_conf(struct log_conf_t *conf);


void log_main(const char *file, int line, int level, const char *subsystem, const char *format, ...);
#define LOG(level, subsystem, args...)                                           \
    log_main(__FILE__, __LINE__, level, subsystem, args);

#ifdef __cplusplus
}
#endif

#endif
