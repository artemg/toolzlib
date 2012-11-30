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
#include <toolz/event.h>

#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>

using namespace std;


struct status_t{
    uint64_t hitcount;
    int time_sum;
    double min_exec_time;
    double max_exec_time;
    double avg_exec_time;
    uint64_t more_1sec_exec_time;
    uint64_t prev_period_req;
    uint64_t cur_period_req;
    int period_start_time;
};

#define LZ_HTTPD_FLAG_DEFAULT 1
#define LZ_HTTPD_FLAG_URL_MATCH_EXACT 2
#define LZ_HTTPD_FLAG_URL_START_WITH 4
#define LZ_HTTPD_FLAG_URL_REGEX 8
#define LZ_HTTPD_FLAG_SIGNAL 16

struct eventMapNode
{
    const char *name;
    void (*func)(lz_httpd_req_t *);
    uint32_t    flags;
    status_t    stat;
};



class CHttpd
{
    public:
enum lz_http_method_t {
  EVHTTP_REQ_GET = 1 << 0, EVHTTP_REQ_POST = 1 << 1, EVHTTP_REQ_HEAD = 1 << 2, EVHTTP_REQ_PUT = 1 << 3,
  EVHTTP_REQ_DELETE = 1 << 4, EVHTTP_REQ_OPTIONS = 1 << 5, EVHTTP_REQ_TRACE = 1 << 6, EVHTTP_REQ_CONNECT = 1 << 7,
  EVHTTP_REQ_PATCH = 1 << 8
};
        CHttpd();
        ~CHttpd();

        enum print_type_t { E_HTML, E_KV };

        int Init(eventMapNode *eventMap, void *event_base);
        int Init(eventMapNode *eventMap);
        void run();
        void run_async();
        int accept(int socket);
        int accept(const char *str, void *args);

        int add_destination(const char *addr, int port, int timeout);
        int send_reply(lz_httpd_req_t *req);
        lz_httpd_req_t *new_request(void (*callb)(lz_httpd_req_t *req, void *arg), void *arg);
        int make_request(int destination, lz_httpd_req_t *req, lz_http_method_t http_type, const char *query);
        int make_request(const char *addr, int port, int conn_timeout, lz_httpd_req_t *req, lz_http_method_t http_type, const char *query);
        int print_common_status(lz_httpd_req_t *req) __attribute__ ((deprecated));
        int print_common_status_html(lz_httpd_req_t *req);
        int print_common_status_kv(lz_httpd_req_t *req);
        int print_common_status_main(lz_httpd_req_t *req, print_type_t type);
        void print_actions(lz_httpd_req_t *req);
        // shutdown callback
        void destroy();
        void shutdown();

        static void request_callb(struct evhttp_request *req, void *arg);

        static const char *get_header(lz_httpd_req_t *req, const char *name);
        static const char *get_query_param(lz_httpd_req_t *req, const char *name);
	static lz_http_method_t get_method(lz_httpd_req_t *req);
	static const char *get_method_str(lz_httpd_req_t *req);
        static void add_header(lz_httpd_req_t *req, const char *name, const char *value);
        static int add_data(lz_httpd_req_t *req, const char *data, size_t data_len);
        static void set_response_code(lz_httpd_req_t *req, int status_code);
        static int add_printf(lz_httpd_req_t *req, const char *fmt, ...);
        static int get_response_code(lz_httpd_req_t *req);
        static int get_post_data(lz_httpd_req_t *req, size_t *size, char **data);


        event_base *get_evbase()  __attribute__ ((deprecated)) {
            return ev_base;
        }

        int custom_perf_counter_add_stat(size_t id, double exec_time);
        size_t custom_perf_counter_add(std::string name);
    private:
        struct event_base *ev_base;
        struct evhttp *ev_http;
        eventMapNode *_eventMap;

        static void dispatch(struct evhttp_request *evreq, void *arg);


        void process_request(eventMapNode *event, lz_httpd_req_t *lz_req);

        lz_httpd_req_t *get_free_req();
        void push_free_req(lz_httpd_req_t *req);




        int update_statistic(status_t *stat, double exec_time); 

        struct destination_t {
            const char *addr;
            int port;
            int conn_timeout;
            status_t stat;
        };
        std::vector<destination_t> destinations;
        
        struct custom_perf_counter_el_t{
            std::string name;
            status_t stat;            
        };
        typedef std::vector<custom_perf_counter_el_t> custom_perf_counter_t;
        custom_perf_counter_t custom_perf_counters;



        void eShowActions(lz_httpd_req_t *req);
        timeval start_time;
};


#endif

