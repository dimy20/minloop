#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "../include/stream.h"
#include "../include/net.h"
#include "../include/core.h"
#include "../include/errno.h"
#include "../include/error.h"

	


void server_cb(io_core_t * ioc, uint8_t status){
	stream_t  * server;
	if(status & EV_CONNECTION){
		server = container_of(ioc, stream_t, io_ctl);
		server->on_connection(NULL);
		printf("calling on_connection!");
	}
}

int stream_init(loop_t * loop, stream_t * stream){
	assert(stream != NULL && "stream pointer is NULL");
	memset(stream, 0, sizeof(stream_t));
	int err;

	io_core_init(&stream->io_ctl, IO_OFF, 0, server_cb);
	err = qc_buffer_init(&stream->bufs[IN_BUFF], 0);
	if(err < 0)
		return err;

	err = qc_buffer_init(&stream->bufs[OUT_BUFF], 0);
	if(err < 0)
		return err;

	stream->on_connection = NULL; /*this will go in stream_listen*/

	return OP_SUCCESS;
}



int stream_server(loop_t * loop, stream_t * stream, char * hostname, char * port){
	assert(loop != NULL && "loop_t pointer is NULL");
	assert(stream != NULL && "stream_t pointer is NULL");

	int err;

	/*what should the loop do with this io when this happens?*/
	if(io_core_fd(&stream->io_ctl) != IO_OFF)
		LOG_ERROR(EIO_BUSY);

	err = ntcp_server(hostname, port);

	if(err < 0)
		return err;

	iocore_setfd(&stream->io_ctl, err);

	err = loop_start_io(loop, &stream->io_ctl);

	if(err < 0)
		LOG_ERROR(err);

	return err;
}

int stream_listen(loop_t * loop, stream_t * stream, connection_cb on_connection){
	assert(loop != NULL && "loop_t pointer is NULL");
	assert(stream != NULL && "stream_t pointer is NULL");
	int err;

	if(on_connection == NULL)
		return -EINVAL;

	stream->on_connection = on_connection;

	err = ntcp_listen(io_core_fd(&stream->io_ctl), 10);

	if(err < 0){
		return err;
	}

	return OP_SUCCESS;
}



