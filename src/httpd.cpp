#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <netinet/tcp.h>

#include <toolz/httpd.h>
#include <toolz/net.h>
#include <toolz/event.h>

#include <event2/event.h>
#include <event2/util.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/http_struct.h>
#include <event2/thread.h>
#include <event2/event_struct.h>

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW 4
#endif


CHttpd::CHttpd()
{
    evthread_use_pthreads();
};

CHttpd::~CHttpd()
{
;
};



const char *CHttpd::get_header(lz_httpd_req_t *req, const char *name){
    return evhttp_find_header(evhttp_request_get_input_headers(req->evreq), name);
}

const char *CHttpd::get_query_param(lz_httpd_req_t *req, const char *name){
    if( !req->query_params_parsed ){ 
        evhttp_parse_query(req->evreq->uri, &req->query_params);
        req->query_params_parsed = 1;
    }
    return evhttp_find_header(&req->query_params, name);
}

void CHttpd::add_header(lz_httpd_req_t *req, const char *name, const char *value){
    evhttp_add_header(evhttp_request_get_output_headers(req->evreq), name, value);
}

int CHttpd::add_data(lz_httpd_req_t *req, const char *data, size_t data_len){
    return evbuffer_add(evhttp_request_get_output_buffer(req->evreq), data, data_len);
}


void CHttpd::set_response_code(lz_httpd_req_t *req, int status_code){
    req->response_status_code = status_code;
}

int CHttpd::get_response_code(lz_httpd_req_t *req){
    return evhttp_request_get_response_code(req->evreq);
}

int CHttpd::get_post_data(lz_httpd_req_t *req, size_t *size, char **data){
    struct evbuffer *evb;
    evb = evhttp_request_get_input_buffer(req->evreq);
    *size = evbuffer_get_length(evb);
    *data = (char *)evbuffer_pullup(evb, *size);
    return 0;
}


int CHttpd::add_printf(lz_httpd_req_t *req, const char *fmt, ...){
    int res = -1;
    va_list ap;

    va_start(ap, fmt);
    res = evbuffer_add_vprintf(evhttp_request_get_output_buffer(req->evreq), fmt, ap);
    va_end(ap);

    return (res);
}

int CHttpd::print_common_status_kv(lz_httpd_req_t *req){
    return print_common_status_main(req, E_KV);
}

int CHttpd::print_common_status(lz_httpd_req_t *req){
    return print_common_status_main(req, E_HTML);
}

int CHttpd::print_common_status_main(lz_httpd_req_t *req, print_type_t type){
    struct timeval ti;
    gettimeofday(&ti, NULL);
    int time_offset=ti.tv_sec - start_time.tv_sec;
//    int64_t wdays = time_offset/86400; 
//    int64_t whours = (time_offset - wdays*86400)/3600;
//    int64_t wmin = (time_offset - wdays*86400  - whours*3600) / 60;
//    int64_t wsec = time_offset - wdays*86400  - whours*3600 - wmin*60;
//    char work_str[256];
    int64_t avg_req = 0;
    int period_delta;

//    lx_snprintf(work_str, sizeof(work_str), "%ld days %ld hours %ld minutes %ld seconds", 
//        wdays, whours, wmin, wsec);
//    char work_time[128];
//    if(!lx_strftimestamp(work_time, sizeof(work_time), conf->start_time, "%x %X")) {
//        ERROR_LOG(L_ERROR, "Failed to fill time buffer");
//        return -1;
//    }

//    const map<string, status_val> *status = get_status();
//    map<string, status_val>::const_iterator status_i;
//    if(status->size() > 0) {
    if( type == E_HTML ){
        CHttpd::add_printf(req,
            "<html>\n<meta http-equiv=\"Content-Type\" content=\"text/html;"
            "charset=utf-8\" />\n<head>\n<title>Welcome</title>\n</head>\n<body>\n"
            "<table border=1 cellspacing='0' cellpadding='3'>\n"  
            "<tr><th>action/result</th><th>count</th>"
            "<th>per sec</th><th>per sec in last</th>"
            "<th>min exec time</th><th>max exec time</th>"
            "<th>avg exec time</th><th>exec time > 1s</th></tr>\n");
    }

    // PRINT STATISTIC PER ACTIONS
    for(eventMapNode *c = _eventMap; c->name != NULL; c++){
        status_t *st = &c->stat;
        period_delta = 0;
        if( st->prev_period_req > 0) {
            period_delta += AVG_REQUESTS_SWITCH_PERIOD;
        }
        period_delta += ti.tv_sec - st->period_start_time;
        avg_req = (st->prev_period_req > 0 ? st->prev_period_req : st->cur_period_req);
        if( type == E_HTML ){
            CHttpd::add_printf(req, "<tr><td>%s</td><td>%lu</td><td>%d</td><td>%d minutes: %ld</td>"
                                    "<td>%f</td><td>%f</td><td>%f</td><td>%ld</td></tr>\n",
                    c->name,
                    st->hitcount,
                    (time_offset>0 ? st->hitcount/time_offset : 1),
                    (period_delta)/60,
                    (st->prev_period_req + st->cur_period_req)/(period_delta + 1),// + 1 if it is not well defined
                                                                                  // to prevent devision by zero
                    st->min_exec_time, 
                    st->max_exec_time, 
                    st->avg_exec_time, 
                    st->more_1sec_exec_time);
        } else if ( type == E_KV ){
            CHttpd::add_printf(req, "%s_hit: %lu\n", c->name, st->hitcount);
        }
    }

    // PRINT STATISTIC PER DESTINATIONS
    for(std::vector<destination_t>::iterator i = destinations.begin();
        i != destinations.end();
        ++i)
    {
        status_t *st = &i->stat;
        period_delta = 0;
        if( st->prev_period_req > 0) {
            period_delta += AVG_REQUESTS_SWITCH_PERIOD;
        }
        period_delta += ti.tv_sec - st->period_start_time;
        avg_req = (st->prev_period_req > 0 ? st->prev_period_req : st->cur_period_req);
        if( type == E_HTML ){
            CHttpd::add_printf(req, "<tr><td>%s:%d</td><td>%lu</td><td>%d</td><td>%d minutes: %ld</td>"
                                    "<td>%f</td><td>%f</td><td>%f</td><td>%ld</td></tr>\n",
                    i->addr,
                    i->port,
                    st->hitcount,
                    (time_offset>0 ? st->hitcount/time_offset : 1),
                    (period_delta)/60,
                    (st->prev_period_req + st->cur_period_req)/(period_delta + 1),// + 1 if it is not well defined
                                                                                  // to prevent devision by zero
                    st->min_exec_time, 
                    st->max_exec_time, 
                    st->avg_exec_time, 
                    st->more_1sec_exec_time);
        } else if ( type == E_KV ){
             CHttpd::add_printf(req, "%s:%d_hit: %lu\n", i->addr, i->port, st->hitcount);
        }
    }
    // PRINT STATISTIC PER CUSTOM COUNTERS
    for(custom_perf_counter_t::iterator i = custom_perf_counters.begin();
        i != custom_perf_counters.end();
        ++i)
    {
        status_t *st = &i->stat;
        period_delta = 0;
        if( st->prev_period_req > 0) {
            period_delta += AVG_REQUESTS_SWITCH_PERIOD;
        }
        period_delta += ti.tv_sec - st->period_start_time;
        avg_req = (st->prev_period_req > 0 ? st->prev_period_req : st->cur_period_req);
        if( type == E_HTML ){
            CHttpd::add_printf(req, "<tr><td>%s</td><td>%lu</td><td>%d</td><td>%d minutes: %ld</td>"
                                    "<td>%f</td><td>%f</td><td>%f</td><td>%ld</td></tr>\n",
                    i->name.c_str(),
                    st->hitcount,
                    (time_offset>0 ? st->hitcount/time_offset : 1),
                    (period_delta)/60,
                    (st->prev_period_req + st->cur_period_req)/(period_delta + 1),// + 1 if it is not well defined
                                                                                  // to prevent devision by zero
                    st->min_exec_time, 
                    st->max_exec_time, 
                    st->avg_exec_time, 
                    st->more_1sec_exec_time);
        } else if ( type == E_KV) {
              CHttpd::add_printf(req, "%s_hit: %lu\n", i->name.c_str(), st->hitcount);
        }
    }
    if( type == E_HTML ){
        CHttpd::add_printf(req,"</table>");
    }
    return 0;
}

int CHttpd::accept(const char *bind_str, void *arg){
    int sock = getSocket(bind_str, arg);
    if( sock == -1 ){
        return -1;
    }
    return accept(sock); // TODO CLOSE
}

int CHttpd::accept(int socket){
   return evhttp_accept_socket(ev_http, socket); // TODO CLOSE
}

void CHttpd::shutdown(){
    event_base_loopbreak(ev_base);
}

void CHttpd::Destroy(){
    evhttp_free(ev_http);
}

int CHttpd::Init(eventMapNode *eventMap){
    return Init(eventMap, NULL); 
}


int CHttpd::Init(eventMapNode *eventMap, void *event_base)
{
    int32_t err = 0;

    gettimeofday(&start_time, NULL); 


    // check event map
    if (eventMap[0].name == NULL){
        LOG(L_DEBUG, "toolzlib", "CShttpd::CShttpd eventMap could not be empty\n");
        goto fail;
    }

    _eventMap = eventMap;
    for (eventMapNode *eventMapCursor = _eventMap;
        eventMapCursor->name != NULL;
        eventMapCursor++)
    {
        memset(&eventMapCursor->stat, 0, sizeof(eventMapCursor->stat) );
    }

    if( event_base == NULL ){
        ev_base = event_base_new();
    } else {
        ev_base = (struct event_base *)event_base;
    }
    ev_http = evhttp_new(ev_base);

    evhttp_set_gencb(ev_http, CHttpd::dispatch, this);

    err = 0;


ret:
    return err;
fail:
    err = -1;
    goto ret;
}

void CHttpd::run()
{

    event_base_loop(ev_base, 0);
}


// action is done with this 
int CHttpd::send_reply(lz_httpd_req_t *req){
    evhttp_send_reply(req->evreq, req->response_status_code, "", evhttp_request_get_output_buffer(req->evreq));

    if( req->query_params_parsed ){
        evhttp_clear_headers(&req->query_params);
    }

    timespec cur_time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &cur_time);
    double t = diff_timespec(&req->start_time, &cur_time);
    update_statistic(req->stat, t);

    // now free
    push_free_req(req);
    return 0;
}

void CHttpd::process_request(eventMapNode *event, lz_httpd_req_t *lz_req)
{
    event->func(lz_req);
    // lx_req can be free'd here
}


lz_httpd_req_t *CHttpd::get_free_req(){
    return (lz_httpd_req_t *) malloc(sizeof(lz_httpd_req_t));
}

void CHttpd::push_free_req(lz_httpd_req_t *req){
    free(req);
}


void CHttpd::dispatch(struct evhttp_request *evreq, void *arg){
    uint32_t flags = 0;
    CHttpd *me = (CHttpd *)arg;
    const char *action = NULL;
    eventMapNode *eventMapCursor = NULL;
    lz_httpd_req_t *lz_req = NULL;


    const char str_delims[] = "/";
    char *strbuf_it = strdupa(evreq->uri);

    action       = strsep(&strbuf_it, str_delims);
    action       = strsep(&strbuf_it, str_delims);
    if( action == NULL ){
        goto fail;
    }

    lz_req = me->get_free_req();
    if( lz_req == NULL ){
        goto ret;
    }
    // init request
    lz_req->query_params_parsed = 0;
    lz_req->evreq = evreq;
    clock_gettime(CLOCK_MONOTONIC_RAW, &lz_req->start_time);
    lz_req->stat = NULL;
    lz_req->response_status_code = HTTP_OK; // default return status

    // now check for actions
    for (eventMapNode *eventMapCursor = me->_eventMap;
        eventMapCursor->name != NULL;
        eventMapCursor++)
    {
        if (strcmp(action, eventMapCursor->name) == 0){
            lz_req->stat = &eventMapCursor->stat;
            me->process_request(eventMapCursor, lz_req);
            goto ret;
        }
    }

    // no action found show error
    goto fail;

ret:
    return;
fail:
    evhttp_send_reply(evreq, HTTP_BADREQUEST, "", NULL);
    if( lz_req->query_params_parsed ){
        evhttp_clear_headers(&lz_req->query_params);
    }

    me->push_free_req(lz_req);
    goto ret;
}

void CHttpd::request_callb(struct evhttp_request *req, void *arg){
    lz_httpd_req_t *r = (lz_httpd_req_t *)arg;

    if( r->callb ){
        r->callb(r, r->callb_arg); // we must provide r OR req ???
    }

    timespec cur_time;
    clock_gettime(CLOCK_MONOTONIC_RAW, &cur_time);
    double t = diff_timespec(&r->start_time, &cur_time);
    update_statistic(r->stat, t);

//    evhttp_request_free(r->evreq);
    evhttp_connection_free(r->evcon);
    r->httpd->push_free_req(r);
}

lz_httpd_req_t *CHttpd::new_request(void (*callb)(lz_httpd_req_t *req, void *arg), void *arg){
    // todo free mem
    lz_httpd_req_t *r = get_free_req();
    if( r == NULL )
        return NULL;
    r->evcon = NULL;
    r->first_param = NULL;
    r->query_params_parsed = 0;
    r->callb = callb;
    r->callb_arg = arg;
    r->evreq = evhttp_request_new(CHttpd::request_callb, r);
    r->httpd = this;
    if( r->evreq == NULL )
        return NULL;
    return r;
}

// TODO free req on failed requests
int CHttpd::make_request(const char *addr, int port, int conn_timeout, lz_httpd_req_t *req, lz_http_method_t http_method, const char *query){
    int err;
    // do not update any statistic
    req->stat = NULL;

    // 1.  create connection
    req->evcon = evhttp_connection_base_new(
        ev_base,
        NULL,
        addr,
        port 
    );
    if (req->evcon == NULL){
        LOG(L_DEBUG, "toolzlib", "Cannot create evhttp_connection_new()"
            " for IP: '%s' port: '%d'\n",
            addr,
            port
        );
        return -1;
    }
    // 2. set timeout
    evhttp_connection_set_timeout(req->evcon, conn_timeout);

    // 5. do request
    err = evhttp_make_request(req->evcon, req->evreq,
        (evhttp_cmd_type)http_method, query
    );
    if (err == -1){
        LOG(L_DEBUG, "toolzlib", "Cannot create evhttp_make_request()"
            " for IP: '%s' port: '%d'\n",
            addr,
            port
        );
        return -1;
    }
    return 0;
}


// TODO free req on failed requests
int CHttpd::make_request(int destination, lz_httpd_req_t *req, lz_http_method_t http_method, const char *query){
    if( destination > destinations.size() ){
        LOG(L_ERROR, NULL, "Unknown dest\n");
        return -1;
    }
    destination_t *d = &destinations[destination];
    clock_gettime(CLOCK_MONOTONIC_RAW, &req->start_time);
    req->stat = &d->stat;
    int err;

    // 1.  create connection
    req->evcon = evhttp_connection_base_new(
        ev_base,
        NULL,
        d->addr,
        d->port 
    );
    if (req->evcon == NULL){
        LOG(L_DEBUG, "toolzlib", "Cannot create evhttp_connection_new()"
            " for IP: '%s' port: '%d'\n",
            d->addr,
            d->port
        );
        return -1;
    }
    // 2. set timeout
    evhttp_connection_set_timeout(req->evcon, d->conn_timeout);

    // 5. do request
    err = evhttp_make_request(req->evcon, req->evreq,
        (evhttp_cmd_type)http_method, query
    );
    if (err == -1){
        LOG(L_DEBUG, "toolzlib", "Cannot create evhttp_make_request()"
            " for IP: '%s' port: '%d'\n",
            d->addr,
            d->port
        );
        return -1;
    }
    return 0;
}

void CHttpd::print_actions(lz_httpd_req_t *req){
    add_printf(req,
        "<HTML>\n"
        "<HEAD></HEAD>\n"
        "<BODY>\n"
        "<TABLE>\n"
    );

    add_printf(req,
        "<tr><td>All supported actions</td></tr>\n"
    );
    eventMapNode *eventMapCursor = _eventMap;
    while (eventMapCursor->name != NULL){
        add_printf(req,
            "<tr><td><a href=\"/%s\">/%s</a></td></tr>\n",
            eventMapCursor->name,
            eventMapCursor->name
        );
        eventMapCursor++;
    }
    add_printf(req,
        "</TABLE>\n"
        "</BODY>\n"
        "</HTML>\n"
    );

}

const char *CHttpd::get_method_str(lz_httpd_req_t *req){
	switch (evhttp_request_get_command(req->evreq)){
		case EVHTTP_REQ_GET:
			return "GET";
			break;
		case EVHTTP_REQ_POST:
			return "POST";
			break;
		case EVHTTP_REQ_HEAD:
			return "HEAD";
			break;
		case EVHTTP_REQ_PUT:
			return "PUT";
			break;
		case EVHTTP_REQ_DELETE:
			return "DELETE";
			break;
		case EVHTTP_REQ_OPTIONS:
			return "OPTIONS";
			break;
		case EVHTTP_REQ_TRACE:
			return "TRACE";
			break;
		case EVHTTP_REQ_CONNECT:
			return "CONNECT";
			break;
		case EVHTTP_REQ_PATCH:
			return "PATCH";
			break;
		default:
			return "";
			break;
	}
}

CHttpd::lz_http_method_t get_method(lz_httpd_req_t *req){
	return (CHttpd::lz_http_method_t) evhttp_request_get_command(req->evreq);
}

int CHttpd::add_destination(const char *addr, int port, int timeout){
    destination_t d;
    d.addr = strdup(addr);
    d.port = port;
    d.conn_timeout = timeout;
    memset(&d.stat, 0, sizeof(d.stat) );
    destinations.push_back(d);

    return 0; // TODO more destinations
}

int CHttpd::custom_perf_counter_add_stat(size_t id, double exec_time){
    if( id > custom_perf_counters.size() ){
        return -1;
    }
    return update_statistic(&custom_perf_counters[id].stat, exec_time);
}
size_t CHttpd::custom_perf_counter_add(std::string name){
    custom_perf_counter_el_t t;
    memset(&t.stat, 0, sizeof(t.stat));
    t.name = name;
    custom_perf_counters.push_back(t);
    return custom_perf_counters.size() - 1;
}
