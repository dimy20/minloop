#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>


#include "../include/net.h"
#include <error.h>

int check(int ret,char * err_msg,u_int8_t exflg){
    if(ret < 0){
        perror(err_msg);
        if((exflg & EXIT_FAILURE))
            exit(EXIT_FAILURE);
    }
    return 0;
}

int file_exists(char * filename){
    int ret = 1;
    struct stat s;
    ret = stat(filename,&s);
	if(ret< 0){
		if(!(errno & ENOENT)){
			perror("stat");
		}
	}
    return (ret == 0);
}

int net_tcp_server(char * port){
    struct addrinfo hints, * servinfo, * p;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    int ret, fd, yes = 1;

    ret = getaddrinfo(NULL,port,&hints,&servinfo);
	if(ret < 0){
		perror("net.c:getaddrinfo");
		exit(EXIT_FAILURE);
	}

    /*
    if((status = getaddrinfo(NULL,port,&hints,&servinfo)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    }
    */

    for(p = servinfo; p!= NULL;p = p->ai_next){
        fd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
		if(fd < 0){
			perror("net.c:socket");
			continue;

		}

        ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int));

		if(ret < 0){
			perror("net.c:setcokop");
			exit(EXIT_FAILURE);
		}

        ret = bind(fd, p->ai_addr, p->ai_addrlen);

		if(ret < 0){
			perror("net.c:bind");
			continue;
		}

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(EXIT_FAILURE);
    }

    ret = listen(fd,BACKLOG);
	if(ret < 0){
		perror("net.c:listen");
		exit(EXIT_FAILURE);
	}

    freeaddrinfo(servinfo);

    return fd;
}




int fd_set_nonblocking(int fd){
    if(fd < 0) return -1;
    int flags = fcntl(fd,F_GETFL,0);
    if (flags == -1) return -1;
    flags |= O_NONBLOCK;
    return fcntl(fd,F_SETFL,flags);
}

int net_create_server_usock(char * socket_name){

    struct sockaddr_un name_addr;
    int ret;
    int usocket_fd;


    /*get a unix socket*/
    usocket_fd = socket(AF_UNIX,SOCK_STREAM,0);
    check(usocket_fd,"net.c:socket",EXIT_FAILURE);

    /*For portability reasons*/
    memset(&name_addr,0,sizeof(struct sockaddr_un));

    name_addr.sun_family = AF_UNIX;
    strncpy(name_addr.sun_path, socket_name, sizeof(name_addr.sun_path) - 1);

    /*Bind the unix socket to a name*/

    if(file_exists(socket_name))
        unlink(socket_name);

    ret = bind(usocket_fd, (const struct sockaddr *) &name_addr,sizeof(name_addr));

    check(ret,"net.c:bind",EXIT_FAILURE);

    /*start to listen on this socket.*/
    ret = listen(usocket_fd, BACKLOG);
    check(ret,"net.c:listen",EXIT_FAILURE);

    return usocket_fd;
}

int net_connect(char * host, uint16_t port){
    struct sockaddr_in addr;
    int ret;
    int fd;

    memset(&addr, 0, sizeof(addr));

    fd = socket(AF_INET,SOCK_STREAM,0);
    check(fd,"net.c:socket",EXIT_FAILURE);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host);
    addr.sin_port = htons(port);

    ret = connect(fd,(const struct sockaddr *) &addr,sizeof(addr));
    check(ret,"net.c:connect",EXIT_FAILURE);

    return fd;
}

int net_connect_usock(char * sock_name){
   struct sockaddr_un addr;
   int ret;
   int usocket_fd;

   /* Create local socket. */

   usocket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
   check(usocket_fd,"net.c:socket",EXIT_FAILURE);

   memset(&addr, 0, sizeof(addr));

   /* Connect socket to local socket address. */

   addr.sun_family = AF_UNIX;
   strncpy(addr.sun_path, sock_name , sizeof(addr.sun_path) - 1);

   ret = connect(usocket_fd,(const struct sockaddr *) &addr,sizeof(addr));
   check(ret,"net.c:connect",EXIT_FAILURE);

   return usocket_fd;
}
