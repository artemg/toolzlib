#ifndef TOOLZ_HTTPD_REQ_H
#define TOOLZ_HTTPD_REQ_H

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <sys/queue.h>
#include <event.h>
#include <evhttp.h>
#include <sys/stat.h>
#include <time.h>

#include <cstring>
#include <cstdlib>

#include <arpa/inet.h>
#include <assert.h>

using namespace std;

class CHttpd;

struct lz_httpd_req_t{
    evhttp_connection *evcon;
    struct evhttp_request *evreq;
    //lz_pstr_t first_param;
    const char *first_param;

    struct evkeyvalq query_params;
    int query_params_parsed:1;
    void (*callb)(lz_httpd_req_t *req, void *arg);
    void *callb_arg;
    struct timespec start_time;
    struct status_t *stat; //status to be updated 
    int response_status_code;
    CHttpd *httpd; // base of request
};
/*
httpd_req_init();
httpd_req_destroy();

struct httpd_req {
    public:
        // init request
        int32_t init(uint32_t _flags = PROCESS_SYNC);

        // free request
        void destroy();

        // request attributes {
        struct evhttp_request   *evreq;
        struct evbuffer         *evout;
        void                    *arg;
        uint32_t                status_code;
        char                    *status_line;
        char                    *ip;
        char                    *uri;
        char                    *geo;
        char                    *referer;
        char                    *user_agent;
        void                    *opaq_data;
        evbuffer                *debug_buf;
        uint32_t                debug_level;   

        void *context;
        // request attributes }

    private:
        timeval start_time;

        void do_clear(){
            // request info {
            memset(&start_time, 0, sizeof(start_time));
            parts                   = NULL;
            evreq                   = NULL;
            evout                   = NULL;
            arg                     = NULL;
            status_code             = 500;
            status_line             = NULL;
            ip                      = NULL;
            uri                     = NULL;
            geo                     = NULL;
            referer                 = NULL;
            pool_conn               = NULL;
            debug_buf               = NULL;
            debug_alg_id            = 0;
            debug_level             = 0;
            // request info }

            flags = PROCESS_SYNC;
            send_reply_done = false;
            psess_id = 0;

            http_debug_output = 0;
        }
};
*/

#endif

