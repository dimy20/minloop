#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "include/net.h"
#include "include/loop.h"
#include "include/stream.h"

void on_connection(stream_t * server){
	printf("what??\n");
}

void example_stream(void){
	loop_t loop;
	stream_t server;
	int err;

	loop_init(&loop);
	stream_init(&server);
	stream_server(&loop, &server, "localhost", "8080");

	err = stream_listen(&loop, &server, on_connection);

}

int main(){
	/*
	int fd, peer, err;

	fd = ntcp_server("localhost","8080");
	err = ntcp_listen(fd, 10);
	if(err < 0)
		printf("error!1!!\n");
	printf("listening for connections\n");

	while(1){
		peer = accept(fd, NULL, NULL);
		if(peer == -1){
			if(errno != EAGAIN){
				perror("accept");
			}
		}else{
			printf("New connection : %d \n", peer);
		}
	}

	*/
	example_stream();
	return 0;
}
