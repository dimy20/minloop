#include <sys/epoll.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "../include/loop.h"
#include "../include/error.h"
#include "../include/queue.h"

void loop_init(loop_t * loop){
    assert(loop != NULL && "loop is NULL"); 
    int ret;

    memset(loop, 0, sizeof(loop_t));
	loop->pending_q = malloc(sizeof(queue_t));

    queue_init(loop->pending_q);

    ret = epoll_create1(0);
    error_exit(ret, "Failed to create epoll instance");

	vector_init(&loop->io_watchers, 0);

	loop->fd_count = 0;
    loop->efd = ret;

    loop->poll_fds = NULL;
}

/*If loop has been allocated on the heap, its the job of the
 * caller to free its memory.*/

void loop_free(loop_t * loop){
    assert(loop != NULL && "loop is NULL"); 

	/*free all if queue has nodes at this moment*/
	while(!queue_empty(loop->pending_q)){
		qnode_t * aux;
		aux = queue_pop(loop->pending_q);
		(void)qnode_val(aux);
	};

	free(loop->pending_q);

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
    int ret;

    memset(&ev, 0, sizeof(struct epoll_event));

    /*Add all io_cores waiting to be included in the loop*/
	
    while(!queue_empty(loop->pending_q)){
        qnode_t * node;
        io_core_t * ioc;

        node = queue_pop(loop->pending_q);
        ioc = (io_core_t *)qnode_val(node);
        
        assert(ioc != NULL && "io_core_t pointer is NULL");
        loop_watch_io(loop, ioc);

    }


	
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

void loop_watch_io(loop_t * loop, io_core_t * ioc){
    struct epoll_event ev;
    int ret;

    memset(&ev, 0, sizeof(ev));
    ev.data.fd = ioc->fd;
    ev.events = ioc->events;
	
    ret = epoll_ctl(loop->efd, EPOLL_CTL_ADD, ioc->fd, &ev);

	if(ret == -1){
		error_log("failed to add io_core to loop");
	}

    error_exit(ret, "epoll_ctl");
}

void io_start(loop_t * loop, io_core_t *ioc){
    assert(ioc != NULL && "io_core_t * is NULL");
    queue_insert(loop->pending_q, ioc);

};

int loop_accept(loop_t * loop, io_core_t * server, io_core_t * peer){
    assert(loop != NULL && "loop_t pointer loop is NULL");

	int ret;
	ret = __io_accept(server->fd, peer);
	if(ret < 0 && ret == -EIO_ACCEPT_FAIL){
		perror("accept() -> Failed to accept connection\n");
	}

	return OP_SUCCESS;
	
}
