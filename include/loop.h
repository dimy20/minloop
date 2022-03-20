#ifndef LOOP_H
#define LOOP_H

#include <sys/epoll.h>
#include "core.h"
#include "queue.h"
#include "vector.h"

#define MAX_EVENTS 16
#define VEC_DEFAULT 32 
#define TEMP_TIMEOUT 50/*This will be removed later*/

#define EV_ERROR	  1
#define EV_READ       2
#define EV_WRITE      3
#define EV_CONNECTION 4
#define EV_CLOSE      5

typedef struct loop_s loop_t;

struct loop_s {
    int efd;
	queue_t * cleanup_q; /*unhealty resources will be queued here for clean up*/
    queue_t * pending_q;
	queue_t * write_q;
	vector_t io_watchers;
	int fd_count;
};

void loop_init(loop_t * loop);
void loop_free(loop_t * loop);
void loop_start(loop_t * loop);
void poll_io(loop_t * loop);
void loop_run_cb(loop_t * loop, int fd);
int loop_watch_io(loop_t * loop, io_core_t * ioc);
int loop_start_io(loop_t * loop, io_core_t * ioc);
int loop_clean_up(loop_t * loop);

#endif

