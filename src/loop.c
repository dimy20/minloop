#include <sys/epoll.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>


#include "../include/loop.h"
#include "../include/error.h"
#include "../include/queue.h"

void loop_init(loop_t * loop){
    assert(loop != NULL && "loop is NULL"); 
    int ret;

    memset(loop, 0, sizeof(loop_t));
    queue_init(loop->pending_q);

    ret = epoll_create1(0);
    error_exit(ret, "Failed to create epoll instance");

    loop->efd = ret;
    loop->io_watchers = NULL;
    loop->poll_fds = NULL;
}



void loop_start(loop_t * loop){
    assert(loop != NULL && "loop is NULL"); 


    while(1){
        printf("loop running\n");
        poll_io(loop);
    }
}

void poll_io(loop_t * loop){
    assert(loop != NULL && "loop is NULL"); 
    struct epoll_event ev[MAX_EVENTS];

    memset(&ev, 0, sizeof(struct epoll_event));

    int ret;

    ret = epoll_wait(loop->efd, ev, MAX_EVENTS, TEMP_TIMEOUT);
    error_exit(ret, "epoll_wait");

    for(int i = 0; i < ret; i++){
        if(ev[i].events & EPOLLIN){
            printf("EPOLLIN event \n");
        }

        if(ev[i].events & EPOLLOUT){
            printf("EPOLLOUT event \n");
        }
    }

}

void loop_run_cb(loop_t * loop, int fd){
    assert(loop != NULL && "loop is NULL"); 
    struct epoll_event ev[MAX_EVENTS];

    memset(&ev, 0, sizeof(struct epoll_event));

    int ret;

    ret = epoll_wait(loop->efd, ev, MAX_EVENTS, TEMP_TIMEOUT);
    error_exit(ret, "epoll_wait");

    for(int i = 0; i < ret; i++){
        if(ev[i].events & EPOLLIN){
            printf("EPOLLIN event \n");
        }

        if(ev[i].events & EPOLLOUT){
            printf("EPOLLOUT event \n");
        }
    }

}

void loop_watch_fd(loop_t * loop, int fd){
    struct epoll_event ev;
    int ret;

    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;

    ret = epoll_ctl(loop->efd, EPOLL_CTL_ADD, fd, &ev);
    error_exit(ret, "epoll_ctl");
    
}

void io_start(loop_t * loop, io_core_t *ioc){
    assert(ioc != NULL && "io_core_t * is NULL");
    queue_insert(loop->pending_q, &ioc->fd);
};

