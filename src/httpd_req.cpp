#include <toolz/httpd_req.h>


lx_req::lx_req(CShttpd *_chttpd)
{
    chttpd = _chttpd;
    assert(chttpd != NULL);
    do_clear();
}

int32_t lx_req::do_init (uint32_t _flags)
{
    //start exec time
    gettimeofday(&start_time, NULL);
    // Don't forget setup reply handler
    // in "send_reply" method

    flags = _flags;
    int32_t err = -1;

    // allocate evbuffer
    if (evout == NULL){
        evout = evbuffer_new();
        if (evout == NULL){
            ERROR_LOG(L_ERROR, "Cannot allocate evbuffer_new()\n");
            goto fail;
        }
    } else {
        ERROR_LOG(L_ERROR, "Attempt double allocate evbuffer_new()\n");
    }


    // all ok
    err = 0;

    ret:
        return err;
    fail:
        err = -1;
        goto ret;
}

void lx_req::do_free ()
{
    if (parts != NULL){
        delete parts;
        parts = NULL;
    }
    if (evout != NULL){
        evbuffer_free(evout);
        evout = NULL;
    }
    if (thread != NULL){
        delete thread;
        thread = NULL;
    }
    if (status_line != NULL){
        free(status_line);
        status_line = NULL;
    }
    if(pool_conn!=NULL) {
        pool_conn->relese();
    }
    if(debug_buf!=NULL) {
        evbuffer_free(debug_buf);
        debug_buf = NULL;
    }
    do_clear();
};

