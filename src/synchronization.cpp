#include "toolz/synchronization.h"
#include "toolz/log.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/eventfd.h>
#include <pthread.h>
#include <stdint.h>

#define _GNU_SOURCE
#include <poll.h>

static void *callb(void *arg){
    static const uint64_t b = {1};
    callb_arg *c = (callb_arg *) arg;
    c->f(c->arg);
    write(c->eventfd, &b, sizeof(b));
    close(c->eventfd);
    free(c);
    return NULL;
}

// -1 on error
// 1 on timeout
// 0 on ok
int callwithtimeout(func *f, void *farg, struct timespec *ts){
    struct pollfd pp;
    int local_eventfd;
    int ret;
    pthread_t t;
    pthread_attr_t attr;
    callb_arg *arg = (callb_arg *)malloc(sizeof(callb_arg));
    arg->f = f;
    arg->arg = farg;
    arg->eventfd = eventfd(0, EFD_NONBLOCK);

    if( arg->eventfd == -1 ){
        goto error_free;
    }
    local_eventfd = dup(arg->eventfd);
    if( local_eventfd == -1 ){
        goto error_free;
    }
    
    pp.fd = local_eventfd;
    pp.events = POLLIN;

    if( pthread_attr_init(&attr) ){
        LOG(L_ERROR, NULL, "Cant init thread attr\n");
        goto error_close_and_free;
    }
    if( pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED) ){
        LOG(L_ERROR, NULL, "Cant set thread sched\n");
        goto error_close_and_free;
    }
    if( pthread_create(&t, &attr, callb, arg) ){
        LOG(L_ERROR, NULL, "Cant create thread\n");
        goto error_close_and_free;
    }
    if( pthread_attr_destroy(&attr) ){
        LOG(L_ERROR, NULL, "Cant destroy attr thread\n");
        goto error;
    }

    ret = ppoll(&pp, 1, ts, NULL);
    if( ret != 1 || !(pp.revents & POLLIN) ){
        close(local_eventfd);
        return 1;
    } 
    close(local_eventfd);
    return 0;

error_close_and_free:
    close(arg->eventfd);
    close(local_eventfd);
error_free:
    free(arg);
error:
    return -1;
}
