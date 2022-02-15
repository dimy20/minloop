#include "../include/loop.h"
#include <sys/epoll.h>
#include <assert.h>
#include <stdio.h>


void loop_init(loop_t * loop){
    assert(loop != NULL && "loop is NULL"); 
    int ret;

    ret = epoll_create1(0);
    error_exit(ret, "Failed to create epoll instance");

    loop->efd = ret;

    loop->io_watchers = NULL;
    loop->pending = NULL;
    loop->poll_fds = NULL;
}

