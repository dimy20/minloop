#ifndef NET_H
#define NET_H

#include <stdint.h>

#define EXIT_F 0X01
#define BACKLOG 20
int new_socket(int family, int socktype, int protocol);

int ntcp_listen(int fd, int backlog);

int ntcp_server(const char * hostname, const char * port);

int nsock_nonblock(int fd);
/*Creates a unix domain socket for listening and binds it to sock_name*/
int net_create_server_usock(const char * sock_name);
/*Connects to other end of unix domain socker named sock_name*/
int nconnect_unix(const char * sock_name);

int net_connect(const char * host, uint16_t port);

int file_exists(const char * filename);

int issocket_bound(int fd);

int nepoll_ctl(int efd, int op, int fd, uint32_t events);
#endif
