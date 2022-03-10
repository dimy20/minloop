#ifndef STREAM_H
#define STREAM_H


#include "core.h"
#include "qc_buffer.h"
#include "loop.h"

#define IN_BUFF 0 /*in_buff index*/
#define OUT_BUFF 1 /*out_buff index*/


typedef struct stream_s stream_t;
typedef void (*connection_cb)(stream_t * server);

struct stream_s{
	io_core_t io_ctl;
	qc_buffer_t bufs[2];
	void * PRIVATE;
};


int stream_init(loop_t * loop, stream_t * stream);
/*decide later where the addr and port should be passed + other options*/
int stream_server(stream_t * server, char *hostname, char *port); 
int stream_listen(stream_t * server, connection_cb on_connection);
int stream_accept(stream_t * server , stream_t * peer);

#endif
