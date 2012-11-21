#include <toolz/event.h>
lz_event_base_t lz_event_base_new(){
    return event_base_new();
}
void lz_event_base_run(lz_event_base_t t){
    event_base_loop((event_base*)t, 0);
}

