#include <assert.h>
#include <string.h>
#include "../include/stream.h"
#include "../include/net.h"
#include "../include/core.h"
#include "../include/errno.h"
//#include "../include/loop.h"

	
#define IN_BUFF 0
#define OUT_BUFF 1

void stream_init(loop_t * loop, stream_t * stream){
	assert(stream != NULL && "stream pointer is NULL");
	memset(stream, 0, sizeof(stream_t));

	io_core_init(&stream->io_ctl, IO_OFF, 0, NULL);
	qc_buffer_init(&stream->bufs[IN_BUFF], 0);
	qc_buffer_init(&stream->bufs[OUT_BUFF], 0);
	stream->on_connection = NULL; /*this will go in stream_listen*/
}



int stream_server(loop_t * loop, stream_t * stream, char * hostname, char * port){
	assert(loop != NULL && "loop_t pointer is NULL");
	assert(stream != NULL && "stream_t pointer is NULL");

	int err, ret;

	if(stream->io_ctl.fd != IO_OFF)
		return -EIO_BUSY;

	err = ntcp_server(hostname, port);

	if(err < 0)
		return err;

	stream->io_ctl.fd = err;

	ret = loop_start_io(loop, &stream->io_ctl);

	if(ret < 0 && ret == -EIO_START)
		return ret;

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



