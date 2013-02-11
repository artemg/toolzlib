#ifndef TOOLZ_NEWLINE_READER_H
#define TOOLZ_NEWLINE_READER_H
#include <event.h>
#include <toolz/event.h>
#include <toolz/profile.h>

typedef void *(* fn)(char *str, size_t s, void *);

class CNewLineReader{
public:
    int Init(lz_event_base_t event_base, fn callb, void *callb_arg, const char *fname);
    int Destroy();

    static void cb2(struct bufferevent *bev, short what, void *ctx);
    static void read_cb(struct bufferevent *be, void *ctx);

    const char *fname;
    lz_event_base_t event_base;
    struct bufferevent *be;
    fn callb;
    void *callb_arg;
    int fd;
    status_t stat;
};

#endif

