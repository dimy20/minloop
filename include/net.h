#ifndef NET_H
#define NET_H

#include <stdint.h>

#define EXIT_F 0X01
#define BACKLOG 20
int new_socket(int family, int socktype, int protocol);

int net_tcp_server(char * hostname, char * port);

int fd_set_nonblocking(int fd);
/*Creates a unix domain socket for listening and binds it to sock_name*/
int net_create_server_usock(char * sock_name);
/*Connects to other end of unix domain socker named sock_name*/
int net_connect_usock(char * sock_name);

int net_connect(char * host, uint16_t port);

int file_exists(char * filename);
#endif
