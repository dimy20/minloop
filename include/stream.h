#ifndef STREAM_H
#define STREAM_H


#include "core.h"
#include "qc_buffer.h"
#include "loop.h"

#define IN_BUFF 0 /*in_buff index*/
#define OUT_BUFF 1 /*out_buff index*/

#define EV_ERROR 0x01
#define EV_READ 0x02
#define EV_CLOSE 0x04
#define EV_CONNECTION 0x08

typedef unsigned char event_t;
typedef struct stream_s stream_t;
typedef void (*connection_cb)(stream_t * server);
typedef void (*data_cb)(stream_t * stream, int ev);

struct stream_s;




int stream_init(loop_t * loop, stream_t * stream);
int stream_server(stream_t * server, char *hostname, char *port); 
int stream_listen(stream_t * server, connection_cb on_connection);
int stream_accept(const stream_t * server , stream_t * peer);
stream_t * stream_new(loop_t * loop);
void stream_free(stream_t * stream);
int stream_read(stream_t * stream, char * buff, int size);
int stream_write(loop_t * loop, stream_t * stream, char * buff, size_t size);
int stream_send_ready(const stream_t * stream);
int stream_close(loop_t * loop, stream_t * stream);
void stream_on_event(stream_t * stream, int ev, data_cb cb);
int stream_has_data(const stream_t * stream);

#endif
