#ifndef TOOLZLIB_NET_H
#define TOOLZLIB_NET_H
#include <sys/types.h>
#include <sys/socket.h>

int lz_get_network_address(const char *addr_str, sockaddr ** addr, size_t *addr_len);

// return socket
// return -1 on error
//
// bind_str examples:
// unix:/tmp.sock
// 127.0.0.1:80
int getSocket(const char *bind_str, void *args);

ssize_t write_once_ti(int fd, void *buf, size_t count, struct timespec *ts);
ssize_t read_once_ti(int fd, void *buf, size_t count, struct timespec *ts);

int connect_ti(int fd, const struct sockaddr *addr,
    socklen_t addrlen, struct timespec *ts);
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

