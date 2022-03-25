#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

#include "include/net.h"
#include "include/loop.h"
#include "include/stream.h"
#include "include/error.h"
#include "include/timer.h"

loop_t loop;

#define SIZE 512

void on_data(stream_t * peer, int size){
	char * buff = malloc(sizeof(char) * size);
	memset(buff, 0, size);
	int err;

	//printf("Total available to read!: %d\n", size);
	while(stream_has_data(peer)){
		err = stream_read(peer, buff, size);
		//printf("%s\n", buff);
		memset(buff, 0, size);
	}
	/*handle error*/

	free(buff);
}


void handle_close(stream_t * peer, int ev){
	printf("connection closed \n");
	int err;
	err = stream_close(&loop, peer);
	if(err < 0) LOG_ERROR(err);
		
}

void handle_error(stream_t * peer, int err){
	catch_error(err);
	stream_close(&loop, peer);
}

void on_connection(stream_t * server){
	stream_t * peer;
	int err;

	peer = stream_new(&loop);
	err = stream_accept(server, peer);
	if(err < 0) LOG_ERROR(err);
	stream_on_event(peer, EV_READ, on_data);
	stream_on_event(peer, EV_CLOSE , handle_close);
	stream_on_event(peer, EV_ERROR, handle_error);
	stream_write(&loop, peer, "welcome", strlen("welcome"));
}

void timer_cb_test(){
	printf("timer expired!!!\n");
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

	min_timer_t timer_test;
	timer_init(&timer_test, 10);

	timer_start(&loop, &timer_test, 1000, timer_cb_test);

	loop_start(&loop);
	return 0;
}
