#ifndef TOOLZLIB_NET_H
#define TOOLZLIB_NET_H

// return socket
// return -1 on error
//
// bind_str examples:
// unix:/tmp.sock
// 127.0.0.1:80
int getSocket(const char *bind_str, void *args);

/*
struct event_map_t{
    const char *name;
    void (*func)(void *arg);
};

class Dispatcher{
    int Init(event_map_t *ev);
    dispatch(const char *request_uri);
};
*/
#endif

