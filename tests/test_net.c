#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "acutest.h"
#include <string.h>
#include "../include/net.h"


#define PORT "8080"
#define HOSTNAME "localhost"
#define TEST_BACKLOG 10

void test_new_socket(void){
	int fd;
	fd = new_socket(AF_INET, SOCK_STREAM, 0);
	TEST_ASSERT(fd > 0);
	close(fd);
}

void test_ntcp_server(void){
	struct sockaddr_in addr;
	socklen_t len;
	int fd, err;

	fd = ntcp_server(HOSTNAME, PORT);
	TEST_ASSERT(fd > 0);

	err = getsockname(fd, (struct sockaddr *)&addr, &len);	
	if(err< 0) perror("getsockname");

	TEST_ASSERT(addr.sin_port != 0);
	TEST_ASSERT(inet_ntoa(addr.sin_addr) != NULL);
	TEST_ASSERT(issocket_bound(fd));

}

void test_ntcp_listen(void){
	int fd, err;

	fd = ntcp_server(HOSTNAME, PORT);
	err = ntcp_listen(fd, TEST_BACKLOG);
	TEST_ASSERT(err == 0);
}

TEST_LIST = {
	{"int new_socket(int, int, int)", test_new_socket},
	{"int ntcp_server(char *, char*)", test_ntcp_server},
	{"int ntcp_listen(int, int)", test_ntcp_listen},
	{0}
};
