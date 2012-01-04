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


struct failed_val 
{
    int count;
};


#define LZ_HTTPD_FLAG_DEFAULT 1

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

        int add_destination(const char *addr, int port, int timeout);
        int send_reply(lz_httpd_req_t *req);
        lz_httpd_req_t *new_request(void (*callb)(lz_httpd_req_t *req, void *arg), void *arg);
        int make_request(int destination, lz_httpd_req_t *req, int http_type, const char *query);
        int print_common_status(lz_httpd_req_t *req);

        // shutdown callback
        void destroy();
        void shutdown();

        static void request_callb(struct evhttp_request *req, void *arg);

        static const char *get_header(lz_httpd_req_t *req, const char *name);
        static const char *get_query_param(lz_httpd_req_t *req, const char *name);
        static void add_header(lz_httpd_req_t *req, const char *name, const char *value);
        static int add_printf(lz_httpd_req_t *req, const char *fmt, ...);
        static int get_response_code(lz_httpd_req_t *req);
        static int get_post_data(lz_httpd_req_t *req, size_t *size, char **data);


        event_base *get_evbase()  __attribute__ ((deprecated)) {
            return ev_base;
        }
    private:
        struct event_base *ev_base;
        struct evhttp *ev_http;
        eventMapNode *_eventMap;

        static void dispatch(struct evhttp_request *evreq, void *arg);


        void process_request(eventMapNode *event, lz_httpd_req_t *lz_req);

        lz_httpd_req_t *get_free_req();
        void push_free_req(lz_httpd_req_t *req);

        int update_statistic(const char *statistic_key, double exec_time);

        struct destination_t {
            const char *addr;
            int port;
            int conn_timeout;
        };
        struct status_val{
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
        std::map<std::string, status_val> status;


        std::vector<destination_t> destinations;
        void eShowActions(lz_httpd_req_t *req);
};


#endif

