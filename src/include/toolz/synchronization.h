#ifndef TOOLZ_SYNCHRONIZATION_H
#define TOOLZ_SYNCHRONIZATION_H
#include <poll.h>
typedef void func(void *arg);

struct callb_arg{
    func *f;
    void *arg;
    int eventfd;
};

int callwithtimeout(func *f, void *farg, struct timespec *ts);

#endif

