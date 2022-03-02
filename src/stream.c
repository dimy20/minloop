#include <assert.h>
#include <string.h>
#include "../include/stream.h"
#include "../include/net.h"
//#include "../include/loop.h"

	
#define IN_BUFF 0
#define OUT_BUFF 1

void stream_init(loop_t * loop, stream_t * stream){
	assert(stream != NULL && "stream pointer is NULL");
	memset(stream, 0, sizeof(stream_t));

	io_core_init(&stream->io_ctl, -1, 0, NULL);
	qc_buffer_init(&stream->bufs[IN_BUFF], 0);
	qc_buffer_init(&stream->bufs[OUT_BUFF], 0);
	stream->on_connection = NULL;
}



int stream_server(stream_t * stream, char * hostnmae, char * port){
	assert(stream != NULL && "stream_t pointer is NULL");

	int err;

	err = ntcp_server(NULL, "8080");
	if(err < 0)
		return err;

	stream->io_ctl.fd = err;

	return err;
}



