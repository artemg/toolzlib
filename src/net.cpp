#include "toolz/net.h"
#include "toolz/log.h"

#include <poll.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

int get_max_system_backlog(){
    int ret;
    char b[128];
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE *fp = fopen("/proc/sys/net/core/somaxconn", "r");
    if( fp == NULL )
        return -1;

    read = getline(&line, &len, fp);
    if( read == -1 ){
        ret = -1;
    } else {
        ret = atoi(line);
    }
    if( line )
        free(line);
    fclose(fp);
    return ret;
}

int getSocket(const char *bind_str_all, void *arg){
	int on = 1;
	int flag = 1;
	int sock;
    const char *bind_str = NULL;
    const char *param1   = NULL;
	int backlog = 128;
    const int max_system_backlog = get_max_system_backlog();

    if( bind_str == NULL ){
        return -1;
    }

    const char str_delims[] = " \t";
    char *strbuf_it = strdupa(bind_str_all);

    bind_str       = strsep(&strbuf_it, str_delims);
    param1         = strsep(&strbuf_it, str_delims);
    if( param1 && strncmp(param1, "backlog=", sizeof("backlog=") - 1 ) == 0 ){
        backlog = atoi(param1 + sizeof("backlog=") - 1);
        LOG(L_DEBUG, "toolzlib", "backlog set to %d\n", backlog);
    }

    if( max_system_backlog != -1 && backlog > max_system_backlog ){
        LOG(L_WARN, "toolzlib", "getSocket: trying to set backlog to %d, but truncated to system value %d\n",
            backlog, max_system_backlog);
    }    

	if( strncmp(bind_str, "unix:", sizeof("unix:") - 1 ) == 0 ){
	// bind unix
		sockaddr_un su;
		const char *sock_path = &bind_str[sizeof("unix:") - 1];
		mode_t old_umask;			

		old_umask = umask(0);
		unlink(sock_path);

		sock = socket(AF_UNIX, SOCK_STREAM, 0);
		su.sun_family = AF_UNIX;
		snprintf(su.sun_path, sizeof(su.sun_path), "%s", sock_path);
		if( bind(sock, (const sockaddr*) &su, sizeof(su)) == -1){
			return -1;
		}
		if( listen(sock, backlog) == -1 ){
			return -1;
		}
		umask(old_umask);
	} else {
	// bind tcp
		char *bind_str_cpy = strdupa(bind_str);
		char *port = strchr(bind_str_cpy, ':');
		if( port == NULL ){
			LOG(L_WARN, "toolzlib", "Cant find ':' in bind str '%s'\n", bind_str);
			return -1;
		}
		*port = '\0';
		port++;

		sock = socket(AF_INET, SOCK_STREAM, 0);
		sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(atoi(port));
		sa.sin_addr.s_addr = inet_addr(bind_str_cpy);


		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	//	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	//	setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *)&on, sizeof(on));
	//	setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
		if( bind(sock, (const sockaddr*) &sa, sizeof(sa)) == -1){
			return -1;
		}
		if( listen(sock, backlog) == -1 ){
			return -1;
		}
	}


	int flags = fcntl(sock, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(sock, F_SETFL, flags);
	
	return sock;
}

ssize_t write_once_ti(int fd, void *buf, size_t count, struct timespec *ts){
    int ret;

    struct pollfd pp;
    pp.fd = fd;
    pp.events = POLLOUT;

    ret = ppoll(&pp, 1, ts, NULL);
    if( ret != 1 || !(pp.revents & POLLOUT) ){
        return -2;
    }
    ret = write(fd, buf, count);
    return ret;
}
ssize_t read_once_ti(int fd, void *buf, size_t count, struct timespec *ts){
    int ret;

    struct pollfd pp;
    pp.fd = fd;
    pp.events = POLLIN;

    ret = ppoll(&pp, 1, ts, NULL);
    if( ret != 1 || !(pp.revents & POLLIN) ){
        return -2;
    }
    ret = read(fd, buf, count);
    return ret;
}

int connect_ti(int fd, const struct sockaddr *addr,
    socklen_t addrlen, struct timespec *ts)
{
    struct pollfd pp;
    int ret;
    pp.fd = fd;
    pp.events = POLLOUT;

    int flag = 1;
    flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);

    ret = connect(fd, addr, addrlen);
    if( ret != 0 ){
        return ret;
    }

    ret = ppoll(&pp, 1, ts, NULL);
    if( ret != 1 || !(pp.revents & POLLOUT) ){
        return -2;
    } else {
        socklen_t err_len;
        int error;

        err_len = sizeof(error);
        if(getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &err_len) < 0 || error != 0)
        {
            return -3;
        }
    }
    return 0;
}

