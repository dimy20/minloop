#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "include/net.h"
#include "include/loop.h"
#include "include/stream.h"
#include "include/error.h"

loop_t loop;

void on_connection(stream_t * server){
	stream_t peer;
	int err;

	err = stream_init(&loop, &peer);
	if(err < 0) LOG_ERROR(err);
	err = stream_accept(server, &peer);
	if(err < 0) LOG_ERROR(err);
}


int main(){
	int err;
	stream_t server;

	loop_init(&loop);
	stream_init(&loop, &server);

	err = stream_server(&server, "localhost", "8080");
	if(err < 0) LOG_ERROR(err);

	err = stream_listen(&server, on_connection);
	if(err < 0) LOG_ERROR(err);


	loop_start(&loop);
	return 0;
}
