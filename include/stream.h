#ifndef STREAM_H
#define STREAM_H


#include "core.h"
#include "qc_buffer.h"
#include "loop.h"

#define IN_BUFF 0 /*in_buff index*/
#define OUT_BUFF 1 /*out_buff index*/


typedef struct stream_s stream_t;
typedef void (*connection_cb)(stream_t * server);
typedef void (*data_cb)(stream_t * stream);

struct stream_s;




int stream_init(loop_t * loop, stream_t * stream);
/*decide later where the addr and port should be passed + other options*/
int stream_server(stream_t * server, char *hostname, char *port); 
int stream_listen(stream_t * server, connection_cb on_connection);
int stream_accept(const stream_t * server , stream_t * peer, data_cb on_data);
stream_t * stream_new(loop_t * loop);
void stream_free(stream_t * stream);

#endif
