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
#include <assert.h>
#include <error.h>

#include "../include/net.h"



#define NET_ERR(x) (-(x))

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


int new_socket(int family, int socktype, int protocol){

	int fd, ret;
	fd = socket(family, socktype, protocol);

	if(fd < 0){
		perror("socket");
		return NET_ERR(errno);
	};

	ret = fd_set_nonblocking(fd);
	if(ret < 0){
		perror("failed to make socket non-blocking");
		return NET_ERR(errno);
	}

	return fd;
}

int ntcp_listen(int fd, int backlog){
	int ret;

    ret = listen(fd, BACKLOG);
	if(ret < 0){
		perror("net.c:listen");
		return NET_ERR(errno);
	}

	return ret;
}

int ntcp_server(char * hostname, char * port){
	/*service and node cant be both NULL*/
	if(hostname == NULL && port == NULL)
		return -EINVAL;

    struct addrinfo hints, * servinfo, * p;
    memset(&hints,0,sizeof(hints));

    hints.ai_family = AF_UNSPEC; /*ipv4 or ipv6*/
    hints.ai_flags = AI_PASSIVE; /* wildcard addr if no node is specified*/
    hints.ai_socktype = SOCK_STREAM;
    int ret, fd, yes = 1;

    ret = getaddrinfo(hostname, port, &hints, &servinfo);

	if(ret < 0){
		perror("net.c:getaddrinfo");
		return NET_ERR(errno);
	}
	
	p = servinfo;

	while(p != NULL){
		fd = new_socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if(fd < 0)
			continue;

        ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int));

		if(ret < 0){
			perror("setsockopt");
			return NET_ERR(errno);
		}

		
        ret = bind(fd, p->ai_addr, p->ai_addrlen);

		if(ret < 0){
			perror("net.c:bind");
			continue;
		}

		break;
		p = p->ai_next;
	}


	/*Reached end of list*/
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
		return NET_ERR(errno);
    }

    freeaddrinfo(servinfo);
    return fd;
}




int fd_set_nonblocking(int fd){
	if(fd <= 0)
		return -EINVAL;
	int flags;

    flags = fcntl(fd,F_GETFL,0);
    if (flags < 0)
		return NET_ERR(errno);

    flags |= O_NONBLOCK;
	flags = fcntl(fd,F_SETFL,flags);

    if (flags < 0)
		return NET_ERR(errno);

	return flags;
}

int net_create_server_usock(char * socket_name){
	if(socket_name == NULL)
		return -EINVAL;

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
	if(host == NULL || port <=1024)
		return -EINVAL;

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
   if(sock_name == NULL)
	   return -EINVAL;

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

int issocket_bound(int fd){
	assert(fd >= 0 && "fd must be grater tha 0");

	struct sockaddr_storage addr; 
	socklen_t len = sizeof(addr);
	int is_bound, ret;

	ret = getsockname(fd, (struct sockaddr*)&addr, &len);

	if(ret < 0){
		perror("getsockname");
		return NET_ERR(errno);
	}

	is_bound = (addr.ss_family == AF_INET) && (((struct sockaddr_in *)&addr)->sin_port != 0);
	is_bound |= (addr.ss_family == AF_INET6)&&(((struct sockaddr_in6 *)&addr)->sin6_port != 0);

	return is_bound;
}
int issock_listen(int fd){
	assert(fd >= 0 && "fd is zero or negative");
	int val, err;
	socklen_t len = sizeof(val);
	err = getsockopt(fd, SOL_SOCKET, SO_ACCEPTCONN, &val, &len);
	if(err < 0){
		perror("getsockopt");
		return NET_ERR(errno);
	}
	return err;
}

