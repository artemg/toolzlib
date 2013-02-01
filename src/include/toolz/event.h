#ifndef TOOLZ_EVENT_H
#define TOOLZ_EVENT_H
#include <event.h>

typedef void * lz_event_base_t;

lz_event_base_t lz_event_base_new();
void lz_event_base_free(lz_event_base_t t);
int lz_event_base_run(lz_event_base_t b);
int lz_event_base_break(lz_event_base_t b);


#endif

