#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "../include/stream.h"
#include "../include/net.h"
#include "../include/core.h"
#include "../include/errno.h"
#include "../include/error.h"

struct stream_s{
	io_core_t io_ctl;
	qc_buffer_t bufs[2];
	connection_cb on_connection;
	data_cb on_data;
	int accepted_fd;
};

static void _io_activity_cb(io_core_t * ioc, uint8_t status){
	assert(ioc != NULL && "io_core_t pointer is NULL");

	stream_t * stream;
	stream = container_of(ioc, stream_t, io_ctl);
	if(stream != NULL){
		stream->on_data(stream);
	}
	return;
};

static void server_cb(io_core_t * ioc, uint8_t status){
	printf("calling on_connection!\n");
	assert(ioc != NULL && "io_core_t pointer is NULL");
	stream_t * server;
	int peer_fd;

	server = container_of(ioc, stream_t, io_ctl);

	peer_fd = _io_accept(&server->io_ctl);
	if(peer_fd < 0) return;

	server->accepted_fd = peer_fd;
	server->on_connection(server);
}

int stream_init(loop_t * loop, stream_t * stream){
	assert(stream != NULL && "stream pointer is NULL");
	memset(stream, 0, sizeof(stream_t));
	int err;

	/*init core*/
	io_core_init(&stream->io_ctl, IO_OFF, EPOLLIN | EPOLLET, NULL);

	/*init io buffers*/
	for(int i = 0; i < 2; i++){
		err = qc_buffer_init(&stream->bufs[i], 0);
		if(err < 0) return err;
	}
		
	/*Allocate for private fields*/

	stream->accepted_fd = -1;
	err = loop_start_io(loop, &stream->io_ctl);
	if(err < 0) return err;

	return OP_SUCCESS;
}



int stream_server(stream_t * server, char * hostname, char * port){
	assert(server!= NULL && "stream_t pointer is NULL");

	int err;

	server->io_ctl.cb = server_cb;

	/*what should the loop do with this io when this happens?*/
	if(server->io_ctl.fd != IO_OFF)
		return -EIO_BUSY;

	err = ntcp_server(hostname, port);
	if(err < 0) return err;

	server->io_ctl.fd = err;
	return OP_SUCCESS;
}

int stream_listen(stream_t * server, connection_cb on_connection){
	assert(server != NULL && "stream_t pointer is NULL");
	int err;

	if(on_connection == NULL)
		return -EINVAL;

	server->on_connection = on_connection;

	err = ntcp_listen(iocore_getfd(&server->io_ctl), 10);
	if(err < 0){
		perror("ntcp_listen");
		return err;
	}


	server->io_ctl.status |= STS_LISTEN;

	return OP_SUCCESS;
}

int stream_accept(const stream_t * server , stream_t * peer){
	assert(server != NULL && "stream_t pointer is NULL");
	assert(peer != NULL && "stream_t pointer is NULL");


	if((server->io_ctl.status & STS_LISTEN) != STS_LISTEN)
		return -EIO_ACCEPT_LISTEN;


	if(server->accepted_fd == -1)
		return -EIO_ACCEPT_LISTEN;

	peer->io_ctl.fd = server->accepted_fd;

	return 0;
}

stream_t * stream_new(loop_t * loop){
	assert(loop != NULL && "loop_t pointer is NULL");
	stream_t * new_stream;
	int err;

	new_stream = malloc(sizeof(stream_t));
	if(new_stream == NULL){
		LOG_ERROR(EALLOC);
		return NULL;
	}

	memset(new_stream, 0, sizeof(stream_t));
	err = stream_init(loop, new_stream);
	if(err < 0) LOG_ERROR(err);
		
	return new_stream;
}

void stream_free(stream_t * stream){
	free(stream);
}
