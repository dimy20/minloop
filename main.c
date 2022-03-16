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

void on_data(stream_t * peer, event_t ev){
	char * buff;
	size_t size;
	int err;
	if(ev == EV_READ){
		buff = stream_read(peer, &size);
	}else if(ev == EV_CLOSE){
		err = stream_close(&loop, peer);
		if(err < 0) LOG_ERROR(err);
		printf("peer closed connection\n");
	}
}

void on_connection(stream_t * server){
	stream_t * peer;
	int err;
	peer = stream_new(&loop);
	err = stream_accept(server, peer, on_data);
	if(err < 0) LOG_ERROR(err);
}


int main(){
	int err;
	printf("running\n"); 
	stream_t * server;

	loop_init(&loop);

	server = stream_new(&loop);

	err = stream_server(server,"localhost", "8080");
	if(err < 0) LOG_ERROR(err);

	err = stream_listen(server, on_connection);
	if(err < 0) LOG_ERROR(err);

	loop_start(&loop);
	return 0;
}
