#include <toolz/newline_reader.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


void CNewLineReader::read_cb(struct bufferevent *be, void *ctx){
    size_t size;
    CNewLineReader *t = (CNewLineReader *) ctx;
    char *s = NULL;
    
    while( s = evbuffer_readln(bufferevent_get_input(be), &size, EVBUFFER_EOL_LF) ){
        timespec start_time, end_time;
        clock_gettime(CLOCK_MONOTONIC_RAW, &start_time);

        t->callb(s, size, t->callb_arg);
        free(s);

        clock_gettime(CLOCK_MONOTONIC_RAW, &end_time);
        double diff = diff_timespec(&start_time, &end_time);
        update_statistic(&t->stat, diff);
    }
//    bufferevent_setcb (be, read_cb, NULL, NULL, ctx);
    bufferevent_enable(be, EV_READ);
}

void CNewLineReader::cb2(struct bufferevent *bev, short what, void *ctx){
    CNewLineReader *t = (CNewLineReader *) ctx;
    if( t->fd != -1 )
        close(t->fd);
    if( t->be != NULL )
        bufferevent_free(t->be);
    t->Init(t->event_base, t->callb, t->callb_arg, t->fname);
    //bufferevent_setcb (bev, read_cb, NULL, cb2, ctx);
    //bufferevent_enable(bev, EV_READ );
}

int CNewLineReader::Init(lz_event_base_t event_base_, fn callb_, void *callb_arg_, const char *fname_){
    event_base = event_base_;
    fname = fname_;
    callb = callb_;
    callb_arg = callb_arg_;
    fd = open(fname, O_RDONLY | O_NONBLOCK);
    if( fd == -1 ){
        return -1;
    }
    be = bufferevent_socket_new((struct event_base *)event_base, fd, 0);
    if( be == NULL ){
        return -1;
    }
    bufferevent_setcb (be, read_cb, NULL, cb2, this);
    bufferevent_enable(be, EV_READ );

    return 0;
}

int CNewLineReader::Destroy(){
    if( fd != -1 )
        close(fd);
    if( be != NULL )
        bufferevent_free(be);   
}


