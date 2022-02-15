#ifndef LOOP_H
#define LOOP_H

struct loop_s {
    int epoll_fd;
    io_core_t * io_watchers;
    void * peding;
    int * poll_fds;
}

#endif
