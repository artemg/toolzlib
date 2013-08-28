#ifndef TOOLZLIB_BINARY_WRITER_H
#define TOOLZLIB_BINARY_WRITER_H

#include <event.h>
#include <toolz/event.h>

class CBinaryWriter{
public:
    static void event_cb(struct bufferevent *bev, short what, void *ctx);
    int Init(struct sockaddr *addr, int addr_size, lz_event_base_t base);
    int connect();
    int write(const void *buf, size_t buf_size);
    int get_state();
    uint64_t get_connection_failures();
    size_t get_output_buffer_length();
private:
    lz_event_base_t base;
    struct bufferevent *be;
    int state;
    struct sockaddr *addr;
    int addr_len;
    uint64_t connection_failures;
};
#endif

