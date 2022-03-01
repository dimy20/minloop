#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "acutest.h"
#include <string.h>
#include "../include/net.h"


#define PORT "8080"
void test_new_socket(void){
	int fd;
	fd = new_socket(AF_INET, SOCK_STREAM, 0);
	TEST_ASSERT(fd > 0);
	close(fd);
}
void test_net_tcp_server(void){
	struct sockaddr_in addr;
	socklen_t len;
	int fd, ret;

	fd = net_tcp_server(PORT);

	TEST_ASSERT(fd > 0);

	ret = getsockname(fd, (struct sockaddr *)&addr, &len);

	TEST_ASSERT(addr.sin_port != 0);

	if(ret < 0){
		perror("getsockname");
	}

	/*
	char * ip = inet_ntoa(addr.sin_addr);
	printf("ip bound : %s \n", ip);

	printf("port : %d \n", port);

	if(ret < 0){
		perror("getsockname");
	}
	*/
}



TEST_LIST = {
	{"int new_socket(int, int, int)", test_new_socket},
	{"int net_tcp_server(char *)", test_net_tcp_server},
	{0}
};
