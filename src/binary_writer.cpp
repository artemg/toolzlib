#include <toolz/binary_writer.h>
#include <stdlib.h>
#include <string.h>

#define CONNECTED 0
#define NOT_CONNECTED 1
#define CONNECTING 2

int CBinaryWriter::get_state(){
    return state;
}

uint64_t CBinaryWriter::get_connection_failures(){
    return connection_failures;
}

void CBinaryWriter::event_cb(struct bufferevent *bev, short what, void *ctx){
	CBinaryWriter *me = (CBinaryWriter *)ctx;
	if( what & BEV_EVENT_CONNECTED ){
		me->state = CONNECTED;
	}
	if( what & (BEV_EVENT_READING | BEV_EVENT_WRITING | BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT) ){
		me->state = NOT_CONNECTED;
        me->connection_failures++;
	}
}


int CBinaryWriter::Init(struct sockaddr *addr_, int addr_len_, lz_event_base_t base_){
	base = base_;
	be = NULL;
	addr_len = addr_len_;
	addr = (struct sockaddr *) malloc(addr_len_);
	memcpy(addr, addr_, addr_len);


	state = NOT_CONNECTED;
	connect();
	return 0;
}

// return 0 if there is valid bufferevent
int CBinaryWriter::connect(){
	int ret;
	if( state == CONNECTING || state == CONNECTED ){
		ret = 0;
		goto ret;
	}
	if( be != NULL ){
		bufferevent_free(be);
		be = NULL;
	}
	be =  bufferevent_socket_new((struct event_base *)base, -1, 0);
	if( bufferevent_disable (be, EV_READ) ){
		goto error;
	}
	bufferevent_setcb(be, NULL, NULL, event_cb, this);
	if( bufferevent_socket_connect(be, addr, addr_len) ){
		goto error;
	}
	ret = 0;
	state = CONNECTING;
ret:
	return ret;

error:
	ret = -1;
	if( be != NULL ){
		bufferevent_free(be);
		be = NULL;
	}   
	goto ret;    
}

int CBinaryWriter::write(const void *buf, size_t buf_size){
	if( connect() ){
		return -1;
	}
	if( evbuffer_get_length(bufferevent_get_output(be)) > 1024*1024 )
		return -1;
	return bufferevent_write(be, buf, buf_size);
}
