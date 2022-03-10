#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "../include/stream.h"
#include "../include/net.h"
#include "../include/core.h"
#include "../include/errno.h"
#include "../include/error.h"

	
struct stream_priv_s {
	connection_cb on_connection;
	int accepted_fd;
};

static void server_cb(io_core_t * ioc, uint8_t status){
	printf("calling on_connection!\n");
	assert(ioc != NULL && "io_core_t pointer is NULL");
	stream_t * server;
	int peer_fd;

	server = container_of(ioc, stream_t, io_ctl);
	struct stream_priv_s * priv = (struct stream_priv_s *)server->PRIVATE;

	peer_fd = _io_accept(&server->io_ctl);
	if(peer_fd < 0) return;

	priv->accepted_fd = peer_fd;

	priv->on_connection(server);
}

int stream_init(loop_t * loop, stream_t * stream){
	assert(stream != NULL && "stream pointer is NULL");
	memset(stream, 0, sizeof(stream_t));
	int err;
	void * mem;

	/*init core*/
	io_core_init(&stream->io_ctl, IO_OFF, EPOLLIN | EPOLLET, NULL);

	/*init io buffers*/
	for(int i = 0; i < 2; i++){
		err = qc_buffer_init(&stream->bufs[i], 0);
		if(err < 0) return err;
	}
		
	/*Allocate for private fields*/
	mem = malloc(sizeof(struct stream_priv_s));
	if(mem == NULL) return -EALLOC;

	memset(mem, 0, sizeof(struct stream_priv_s));

	((struct stream_priv_s*)mem)->accepted_fd = -1;

	stream->PRIVATE = mem;

	err = loop_start_io(loop, &stream->io_ctl);
	if(err < 0) LOG_ERROR(err);

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

	((struct stream_priv_s *)server->PRIVATE)->on_connection = on_connection;

	err = ntcp_listen(iocore_getfd(&server->io_ctl), 10);
	if(err < 0)
		return err;

	server->io_ctl.status |= STS_LISTEN;

	return OP_SUCCESS;
}

int stream_accept(stream_t * server , stream_t * peer){
	assert(server != NULL && "stream_t pointer is NULL");
	assert(peer != NULL && "stream_t pointer is NULL");
	int err, peer_fd;

	if((server->io_ctl.status & STS_LISTEN) != STS_LISTEN)
		return -EIO_ACCEPT_LISTEN;

	err = stream_init(peer);
	if(err < 0) return err;

	peer_fd = _io_accept(&server->io_ctl);
	if(peer_fd < 0) return peer_fd;

	/*set returned peer fd*/
	iocore_setfd(&peer->io_ctl, peer_fd);
		
	err = loop_start_io(loop, &peer->io_ctl);
	if(err < 0) return err;

	return 0;
}

