#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "acutest.h"
#include <string.h>
#include "../include/net.h"


#define PORT "8080"
#define HOSTNAME "localhost"

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

	fd = ntcp_server(HOSTNAME, PORT);

	TEST_ASSERT(fd > 0);

	ret = getsockname(fd, (struct sockaddr *)&addr, &len);	
	if(ret < 0){
		perror("getsockname");
	}

	TEST_ASSERT(addr.sin_port != 0);
	TEST_ASSERT(inet_ntoa(addr.sin_addr) != NULL);
	TEST_ASSERT(issocket_bound(fd));
}



TEST_LIST = {
	{"int new_socket(int, int, int)", test_new_socket},
	{"int net_tcp_server(char *)", test_net_tcp_server},
	{0}
};
