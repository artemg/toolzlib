#include "net.h"
#include "log.h"
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <sys/un.h>

int getSocket(const char *bind_str, void *arg){
	int on = 1;
	int flag = 1;
	int sock;
	const int backlog = 128;

	if( strncmp(bind_str, "unix:", sizeof("unix:") - 1 ) == 0 ){
	// bind unix
		sockaddr_un su;
		const char *sock_path = &bind_str[sizeof("unix:") - 1];
		mode_t old_umask;			

		old_umask = umask(0);
		unlink(sock_path);

		sock = socket(AF_UNIX, SOCK_STREAM, 0);
		su.sun_family = AF_UNIX;
		snprintf(su.sun_path, sizeof(su.sun_path), sock_path);
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
			LOG(L_WARN, NULL, "Cant find ':' in bind str '%s'\n", bind_str);
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

