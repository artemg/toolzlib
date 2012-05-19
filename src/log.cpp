#include <toolz/log.h>
#include <syslog.h>

static struct log_conf_t log_conf = {1,1,0,NULL,0};


static int loglevel_to_syslog(int lvl){
    int ret;
    switch (lvl){
        case L_EMERG:
            ret = LOG_EMERG;
            break;
        case L_ALERT:
            ret = LOG_ALERT;
            break;
        case L_CRIT:
            ret = LOG_CRIT;
            break;
        case L_ERROR:
            ret = LOG_ERR;
            break;
        case L_WARN:
            ret = LOG_WARNING;
            break;
        case L_NOTICE:
            ret = LOG_NOTICE;
            break;
        case L_INFO:
            ret = LOG_INFO;
            break;
        case L_DEBUG:
            ret = LOG_DEBUG;
            break;
        default:
            ret = -1;
            break;
    }
    return ret;
}

int set_log_conf(log_conf_t *conf){
    log_conf = *conf;
    if( conf->log_syslog ){
        openlog("daem", LOG_NDELAY, LOG_DAEMON);
    } 
    return 0;
}

// TODO
int set_log_conf_string(const char *str){
    return 0;
}

void log_main(const char *file, int line, int level, const char *subsystem, const char *format, ...){
    va_list va;
    if( level > log_conf.max_log_level 
        && !(log_conf.subsystem && level < log_conf.max_log_subsystem_level
                && subsystem && strcmp(subsystem, log_conf.subsystem) == 0 ) )
    {
        return;
    }
    if( log_conf.log_stderr ){
        fprintf(stderr, "%dAPP[%s](%s:%d) - ", level, subsystem?subsystem:"", file, line);
        va_start(va, format);
        vfprintf(stderr, format, va);
        va_end(va);
    }
    if( log_conf.log_syslog ){
        va_start(va, format);
        vsyslog(loglevel_to_syslog(level), format, va);
        va_end(va);
    }
}

