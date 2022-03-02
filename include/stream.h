#ifndef STREAM_H
#define STREAM_H


#include "core.h"
#include "qc_buffer.h"
#include "loop.h"

typedef void (*connection_cb)(io_core_t * peer);

typedef struct stream_s stream_t;

struct stream_s{
	io_core_t io_ctl;
	connection_cb on_connection;
	qc_buffer_t bufs[2];
};


void stream_init(loop_t * loop, stream_t * stream);
/*decide later where the addr and port should be passed + other options*/
int stream_server(loop_t * loop, stream_t * stream, char *hostname, char *port); 
void stream_listen(loop_t * loop, stream_t * stream, connection_cb on_connection);
	

#endif
