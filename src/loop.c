#include <sys/epoll.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "../include/loop.h"
#include "../include/error.h"
#include "../include/queue.h"
#include "../include/net.h"
#include "../include/timer.h"

static void run_timers(loop_t * loop){
	assert(loop != NULL && "loop_t pointer is NULL");
	int * timeout;
	min_timer_t * timer;
	while(1){
		timeout = heap_min(&loop->timer_heap);
		if(timeout == NULL)
			break;

		if(*timeout > loop->time) /*timer still running*/
			break;

		/*after this point timer is due*/
		timer = container_of(timeout, min_timer_t, timeout);
		if(timer == NULL) printf("error\n");
		timer->cb();
		heap_remove(&loop->timer_heap);
	}
}

void loop_init(loop_t * loop){
    assert(loop != NULL && "loop is NULL"); 
    int ret;

    memset(loop, 0, sizeof(loop_t));
	loop->pending_q = malloc(sizeof(queue_t));
	loop->cleanup_q = malloc(sizeof(queue_t));
	loop->write_q = malloc(sizeof(queue_t));

    queue_init(loop->pending_q);
	queue_init(loop->cleanup_q);
	queue_init(loop->write_q);

	ret = heap_alloc(&loop->timer_heap);
	if(ret < 0) LOG_ERROR(ret);

    ret = epoll_create1(0);
    error_exit(ret, "Failed to create epoll instance");

	vector_init(&loop->io_watchers, 0);

	loop->fd_count = 0;
	loop->efd = ret;
	loop->time = timer_get_ms_time();
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
	free(loop->cleanup_q);
	heap_free(&loop->timer_heap);
}



void loop_start(loop_t * loop){
    assert(loop != NULL && "loop is NULL"); 
	qnode_t * node;
	io_core_t * ioc;
	int timeout;
    while(1){
		while(!queue_empty(loop->write_q)){
			node = queue_pop(loop->write_q);
			ioc = (io_core_t *)qnode_val(node);
			assert(ioc != NULL && "io_core_t pointer is NULL");
			ioc->cb(loop, ioc, EV_WRITE);
		}
		timeout = compute_next_timeout(loop); /*compute for how long should poll block*/
		poll_io(loop, timeout);
		//loop_clean_up(loop);
		loop_update_time(loop);
		run_timers(loop); /*run due timers*/
    }
}

void poll_io(loop_t * loop, int timeout){
    assert(loop != NULL && "loop is NULL"); 

    struct epoll_event ev[MAX_EVENTS];
    int ret, err, fd;
	qnode_t * node;
	io_core_t * ioc;



    /*Add all io_cores waiting to be included in the loop*/
	
    while(!queue_empty(loop->pending_q)){
        node = queue_pop(loop->pending_q);
        ioc = (io_core_t *)qnode_val(node);

        assert(ioc != NULL && "io_core_t pointer is NULL");
		/*start watching this ioc*/
        err = loop_watch_io(loop, ioc);

		if(err < 0){
			if(err == -EIO_LOOP_WATCH){
				/*This io_core_t is now considered unhealty*/
				queue_insert(loop->cleanup_q, ioc);	

				LOG_ERROR(err);
				printf("handle error here\n");

			}
		}
    }

    memset(&ev, 0, sizeof(struct epoll_event) * MAX_EVENTS);

	do{
		ret = epoll_wait(loop->efd, ev, MAX_EVENTS, timeout);
	}while(ret < 0 && errno == EINTR);

    error_exit(ret, "epoll_wait");

    for(int i = 0; i < ret; i++){
		fd = ev[i].data.fd;
		ioc = loop->io_watchers.arr[fd];
        if(ev[i].events & EPOLLIN){
			if(ioc != NULL && ioc->cb != NULL){
				ioc->cb(loop, ioc, EPOLLIN);
			}
        }
        if(ev[i].events & EPOLLOUT){
			if(ioc != NULL && ioc->cb != NULL){
				ioc->cb(loop, ioc, EPOLLOUT);
			}
        }
    }

}

int loop_watch_io(loop_t * loop, io_core_t * ioc){
	assert(loop != NULL && "loop_t pointer is NULL");
	assert(ioc != NULL && "io_core_t pointer is NULL");
	assert(loop->efd != ioc->fd && "epoll should never watch itself");

	/*fd is -1 for some reason*/
	int err;
	if(ioc->fd == IO_OFF){
		return IO_OFF;
	}

	err = nepoll_ctl(loop->efd, EPOLL_CTL_ADD, ioc->fd, ioc->events);
	if(err < 0)
		return -EIO_LOOP_WATCH;

	err = vector_insert(&loop->io_watchers, ioc->fd, ioc);
	if(err < 0)
		return err;

	return OP_SUCCESS;
}

int loop_start_io(loop_t * loop, io_core_t *ioc){
    assert(ioc != NULL && "io_core_t * is NULL");
	void * ret;
    ret = queue_insert(loop->pending_q, ioc);
	/*Assertion makes this unreachable*/
	if( ret == NULL )
		return -EIO_START;

	return OP_SUCCESS;
};

int loop_clean_up(loop_t * loop){
	assert(loop != NULL && "loop_t pointer loop is NULL");
	qnode_t * node;
	io_core_t * ioc;
	while(!queue_empty(loop->cleanup_q)){
		node = queue_pop(loop->cleanup_q);
		ioc = qnode_val(node);
		vector_remove(&loop->io_watchers, ioc->fd);
		ioc->status = 0; //EV_ERROR;
		close(ioc->fd);
	}
	return OP_SUCCESS;
}

void loop_update_time(loop_t * loop){
	assert(loop != NULL && "loop_t pointer loop is NULL");
	loop->time = timer_get_ms_time();
}



