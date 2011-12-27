#ifndef TOOLZ_HTTPD_H
#define TOOLZ_HTTPD_H

#include <string.h>
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


#include <toolz/httpd_req.h>
#include <toolz/profile.h>

#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>

using namespace std;


struct status_val
{
    int hitcount;
    int time_sum;
    double min_exec_time;
    double max_exec_time;
    double avg_exec_time;
    uint64_t more_1sec_exec_time;
    uint64_t prev_period_req;
    uint64_t cur_period_req;
    int period_start_time;
};

struct failed_val 
{
    int count;
};



struct eventMapNode
{
    const char *name;
    void (*func)(lz_httpd_req_t *);
    uint32_t    flags;
};


class CHttpd
{
    public:
        CHttpd();
        ~CHttpd();

        int Init(eventMapNode *eventMap);
        void run();
        void run_async();
        int accept(int socket);
        int accept(const char *str, void *args);

        int send_reply(lz_httpd_req_t *req);

        // shutdown callback
        void destroy();
        void shutdown();

    private:
        struct event_base *ev_base;
        struct evhttp *ev_http;
        eventMapNode *_eventMap;

        static void dispatch(struct evhttp_request *evreq, void *arg);


        void process_request(eventMapNode *event, lz_httpd_req_t *lz_req);

        lz_httpd_req_t *get_free_req();
        void push_free_req(lz_httpd_req_t *req);
};


#endif

