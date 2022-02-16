#ifndef LOOP_H
#define LOOP_H

#include <sys/epoll.h>
#include "core.h"
#include "queue.h"

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
void loop_run_cb(loop_t * loop, int fd);
void loop_watch_fd(loop_t * loop, int fd);
void io_start(loop_t * loop, io_core_t * ioc);

#endif
