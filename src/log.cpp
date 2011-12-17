#include <toolz/log.h>

int set_log_conf(log_conf_t *conf){
    log_conf = *conf;
    /*
    if( log_syslog ){
        openlog();
    } 
    */
    return 0;
};

void log_main(const char *file, int line, int level, const char *subsystem, const char *format, ...){
    va_list va;
    if( level > log_conf.max_log_level 
        && !(log_conf.subsystem && level < log_conf.max_log_subsystem_level
                && subsystem && strcmp(subsystem, log_conf.subsystem) == 0 ) )
    {
        return;
    }

    fprintf(stderr, "%dAPP[%s](%s:%d) - ", level, subsystem, file, line);
    va_start(va, format);
    vfprintf(stderr, format, va);
    va_end(va);

}

