#ifndef LOOP_H
#define LOOP_H

#include "../include/core.h"

typedef struct loop_s loop_t;

struct loop_s {
    int efd;
    io_core_t * io_watchers;
    void * pending;
    int * poll_fds;
};

void loop_init(loop_t * loop);

#endif
