#include <toolz/signals.h>


int add_signal(lz_event_base_t base, int signal, signal_fn callb, void *arg){
    event *ev = evsignal_new((struct event_base *)base, signal, callb, arg);
    evsignal_add(ev, NULL);
    return 0;
}

