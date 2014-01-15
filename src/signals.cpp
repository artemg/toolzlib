#include <toolz/signals.h>
#include <vector>

static std::vector<struct event *> signals;

int add_signal(lz_event_base_t base, int signal, signal_fn callb, void *arg){
    event *ev = evsignal_new((struct event_base *)base, signal, callb, arg);
    signals.push_back(ev);
    return evsignal_add(ev, NULL);
}

int unset_signals(){
    for(std::vector<struct event *>::iterator i = signals.begin();
        i != signals.end();
        ++i)
    {
        evsignal_del(*i);
        event_free(*i);
    }
    return 0;
}

