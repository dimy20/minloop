#ifndef LOOP_H
#define LOOP_H

#include <sys/epoll.h>
#include "core.h"
#include "queue.h"
#include "vector.h"

#define MAX_EVENTS 1024
#define VEC_DEFAULT 32 
#define TEMP_TIMEOUT 0 /*This will be removed later*/

typedef struct loop_s loop_t;

struct loop_s {
    int efd;
    queue_t * pending_q;
	vector_t io_watchers;
	int fd_count;
    int * poll_fds;
};

void loop_init(loop_t * loop);
void loop_free(loop_t * loop);
void loop_start(loop_t * loop);
void poll_io(loop_t * loop);
void loop_run_cb(loop_t * loop, int fd);
int loop_watch_io(loop_t * loop, io_core_t * ioc);
int loop_start_io(loop_t * loop, io_core_t * ioc);


#endif
