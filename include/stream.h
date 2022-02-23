#ifndef STREAM_H
#define STREAM_H
#include "core.h"

typedef void (*connection_cb)(io_core_t * peer);

typedef struct stream_s stream_t;

struct stream_s{
	io_core_t io_ctl;
	connection_cb on_connection;

}
#endif
