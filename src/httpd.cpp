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

#include <event2/event.h>
#include <event2/util.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/http_struct.h>
#include <event2/thread.h>
#include <event2/event_struct.h>

CHttpd::CHttpd()
{
    evthread_use_pthreads();
};

CHttpd::~CHttpd()
{
;
};

int CHttpd::accept(const char *bind_str, void *arg){
    int sock = getSocket(bind_str, arg);
    return accept(sock); // TODO CLOSE
}

int CHttpd::accept(int socket){
   return evhttp_accept_socket(ev_http, socket); // TODO CLOSE
}

void CHttpd::shutdown(){
    event_base_loopbreak(ev_base);
}

int CHttpd::Init(eventMapNode *eventMap)
{
    int32_t i, res, err = 0;
    sockaddr_in  sa;

    int32_t on              =  1;
    int32_t server_sock     = -1;
    //struct linger ling      = {1, conf->timeout_timewait};
    int flag = 1;


    evthread_use_pthreads();
/*
    // check event map
    if (eventMap[0].name == NULL){
        ERROR_LOG(L_ERROR, "CShttpd::CShttpd eventMap could not be empty\n");
        goto fail;
    }
    _eventMap = eventMap;

    // init all requests and mark as free
    for (i = 0; i < MAX_REQ_QUEUE; i++){
        lx_req *lxreq = new lx_req(this);
        requests[i]   = lxreq;
        free_requests.push_back(lxreq);
    }

    // create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        ERROR_LOG(L_ERROR, "Error socket()\n");
        goto fail;
    }

    sa.sin_family      = AF_INET;
    sa.sin_port        = htons(conf->bind_port);
    sa.sin_addr.s_addr = inet_addr(conf->bind_addr);

    res = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (res == -1){
        ERROR_LOG(L_ERROR, "Error setsockopt()\n");
        goto fail;
    }

    res = setsockopt(server_sock, SOL_SOCKET, SO_LINGER, (void *)&ling, sizeof(ling));
    if (res != 0){
        ERROR_LOG(L_ERROR, "setsockopt failed\n");
        goto fail;
    }
    
    res = setsockopt(server_sock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    if (res != 0){
        ERROR_LOG(L_ERROR, "setsockopt failed\n");
        goto fail;
    }

    // Bind server socket to ip:port
    if (bind(server_sock, (const sockaddr*)&sa, sizeof(sa)) == -1) {
        ERROR_LOG(L_ERROR, "Error bind(): for addr: '%s', port: '%d'\n",
            conf->bind_addr,
            conf->bind_port
        );
        goto fail;
    }

    // Make server to listen
    if (listen(server_sock, SERVER_BACKLOG) == -1) {
        ERROR_LOG(L_ERROR, "Error listen()\n");
        goto fail;
    }

    // Init Accesslog
    if (conf->use_access_log_file != NULL){
        string accesslog_path = string(conf->root_dir)
            + string("/")
            + string(conf->use_access_log_file);
        recursivly_mkdir((const char*)accesslog_path.c_str());
        accessLog = new AccessLog(conf, (char *)accesslog_path.c_str(), 32768);
        if(accessLog->Init() < 0 ) {
            ERROR_LOG(L_ERROR, "Failed to init accessLog\n");
            goto ret;
        }
    }

    // Init events
    ev_base = event_base_new();
    ev_http = evhttp_new(ev_base);

    
    serv_base           = (event_base *)event_init();
    this->_http_server  = evhttp_new(serv_base);

    res = evhttp_accept_socket(this->_http_server, server_sock);
    if (res == -1){
        ERROR_LOG(L_ERROR, "Error evhttp_accept_socket()\n");
        goto fail;
    }

    // Set HTTP request callback
    evhttp_set_gencb(this->_http_server, &CShttpd::on_request, (void *)this);

    evhttp_set_timeout(this->_http_server, conf->evhttp_connection_timeout);

    // Dispatch events
    ERROR_LOG(L_NOTE, "http server is running (%s:%d)\n",
        conf->bind_addr, conf->bind_port);

    // store server base to config
    conf->serv_base = serv_base;
*/
    ev_base = event_base_new();
    ev_http = evhttp_new(ev_base);


    err = 0;


ret:
    return err;
fail:
    err = -1;
    goto ret;
}

void CHttpd::run()
{
    evhttp_set_gencb(ev_http, CHttpd::dispatch, this);
    event_base_loop(ev_base, 0);
}


// action is done with this 
int CHttpd::send_reply(lz_httpd_req_t *req){
    evhttp_send_reply(req->evreq, HTTP_OK, "", evhttp_request_get_output_buffer(req->evreq));
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

    lz_httpd_req_t *lz_req = me->get_free_req();
    if( lz_req == NULL ){
        goto ret;
    }

    eventMapCursor = me->_eventMap;
    while (eventMapCursor->name != NULL){
        if (strcmp(action, eventMapCursor->name) == 0){
            flags = eventMapCursor->flags;
            me->process_request(eventMapCursor, lz_req);
            break;
        }
        eventMapCursor++;
    }

    // if we didnt find any node, call last node, that is default
    if (eventMapCursor->name == NULL){
        // now check for addditional actions
        eventMapNode *eventMapCursor = me->_eventMap;
        while (eventMapCursor->name != NULL){
            if (strcmp(action, eventMapCursor->name) == 0){
                flags = eventMapCursor->flags;
                me->process_request(eventMapCursor, lz_req);
                break;
            }
            eventMapCursor++;
        }
    }

    // no action found show error
    goto fail;

ret:
    return;
fail:
    evhttp_send_reply(evreq, HTTP_BADREQUEST, "", NULL);
    goto ret;
}

