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
};


struct stream_priv_s * stream_private(const stream_t * stream){
	assert(stream != NULL && "stream_t pointer is NULL");
	return (struct stream_priv_s *)stream->PRIVATE;
}

static void server_cb(io_core_t * ioc, uint8_t status){
	stream_t * server;
	if(status & EV_CONNECTION){
		server = container_of(ioc, stream_t, io_ctl);
		stream_private(server)->on_connection(server);
		printf("calling on_connection!");
	}
}

int stream_init(stream_t * stream){
	assert(stream != NULL && "stream pointer is NULL");
	memset(stream, 0, sizeof(stream_t));
	int err;

	io_core_init(&stream->io_ctl, IO_OFF, 0, NULL);

	err = qc_buffer_init(&stream->bufs[IN_BUFF], 0);
	if(err < 0) return err;
	err = qc_buffer_init(&stream->bufs[OUT_BUFF], 0);
	if(err < 0) return err;
		
	/*Allocate for private fields*/
	struct stream_priv_s * priv = stream_private(stream);
	priv = malloc(sizeof(struct stream_priv_s));
	if(priv == NULL)
		return -EALLOC;

	priv->on_connection = NULL; /*this will go in stream_listen*/

	return OP_SUCCESS;
}



int stream_server(loop_t * loop, stream_t * stream, char * hostname, char * port){
	assert(loop != NULL && "loop_t pointer is NULL");
	assert(stream != NULL && "stream_t pointer is NULL");

	int err;

	stream->io_ctl.cb = server_cb;
	/*what should the loop do with this io when this happens?*/
	if(iocore_getfd(&stream->io_ctl) != IO_OFF)
		LOG_ERROR(EIO_BUSY);

	err = ntcp_server(hostname, port);
	if(err < 0) return err;

	iocore_setfd(&stream->io_ctl, err);

	err = loop_start_io(loop, &stream->io_ctl);
	if(err < 0) LOG_ERROR(err);
		
	return err;
}

int stream_listen(loop_t * loop, stream_t * stream, connection_cb on_connection){
	assert(loop != NULL && "loop_t pointer is NULL");
	assert(stream != NULL && "stream_t pointer is NULL");
	int err;

	if(on_connection == NULL)
		return -EINVAL;

	stream_private(stream)->on_connection = on_connection;
	err = ntcp_listen(iocore_getfd(&stream->io_ctl), 10);

	if(err < 0){
		return err;
	}

	return OP_SUCCESS;
}

int stream_accept(loop_t * loop, stream_t * server , stream_t * peer){
	assert(loop != NULL && "loop_t pointer is NULL");
	assert(server != NULL && "stream_t pointer is NULL");
	assert(peer != NULL && "stream_t pointer is NULL");
	int err, peer_fd;

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

