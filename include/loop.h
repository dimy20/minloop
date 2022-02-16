#ifndef LOOP_H
#define LOOP_H

#include <sys/epoll.h>
#include "../include/core.h"
#include "../include/queue.h"

#define MAX_EVENTS 1024
#define TEMP_TIMEOUT 0 /*This will be removed later*/

typedef struct loop_s loop_t;

struct loop_s {
    int efd;
    queue_t * pending_q;
    io_core_t * io_watchers;
    int * poll_fds;
};

void loop_init(loop_t * loop);
void loop_start(loop_t * loop);
void poll_io(loop_t * loop);

#endif
