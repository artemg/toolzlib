#ifndef TOOLZ_SIGNALS_H
#define TOOLZ_SIGNALS_H

#include <toolz/event.h>

typedef void(* signal_fn)(int fd, short signal, void *arg);
int add_signal(lz_event_base_t base, int signal, signal_fn callb, void *arg);

#endif

