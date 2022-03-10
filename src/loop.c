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

static int loop_retry_io(loop_t * loop);

void loop_init(loop_t * loop){
    assert(loop != NULL && "loop is NULL"); 
    int ret;

    memset(loop, 0, sizeof(loop_t));
	loop->pending_q = malloc(sizeof(queue_t));
	loop->cleanup_q = malloc(sizeof(queue_t));

    queue_init(loop->pending_q);
	queue_init(loop->cleanup_q);

    ret = epoll_create1(0);
    error_exit(ret, "Failed to create epoll instance");

	vector_init(&loop->io_watchers, 0);
	vector_init(&loop->retry_list, 0);

	loop->fd_count = 0;
    loop->efd = ret;
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
}



void loop_start(loop_t * loop){
    assert(loop != NULL && "loop is NULL"); 
    while(1){
        poll_io(loop);
		loop_clean_up(loop);
    }
}

void poll_io(loop_t * loop){
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

			}else if(err == IO_OFF){
				/*io is currently off, send it to retry list*/
				vector_push_back(&loop->retry_list, ioc);
			}
		}


    }

    memset(&ev, 0, sizeof(struct epoll_event) * MAX_EVENTS);
    ret = epoll_wait(loop->efd, ev, MAX_EVENTS, 0);
    error_exit(ret, "epoll_wait");

    for(int i = 0; i < ret; i++){
        if(ev[i].events & EPOLLIN){
			fd = ev[i].data.fd;
			ioc = loop->io_watchers.arr[fd];
			if(ioc != NULL && ioc->cb != NULL){
				ioc->cb(ioc, EV_CONNECTION);
			}
        }
		/*
        if(ev[i].events & EPOLLOUT){
            printf("EPOLLOUT event \n");
        }
		*/
    }

}

int loop_watch_io(loop_t * loop, io_core_t * ioc){
	assert(loop->efd != ioc->fd && "epoll should never watch itself");

	/*fd is -1 for some reason*/
	if(ioc->fd == IO_OFF){
		return IO_OFF;
	}

    struct epoll_event ev;
	int err;

    memset(&ev, 0, sizeof(ev));
    ev.data.fd = ioc->fd; 
	ev.events = ioc->events; 
	err = epoll_ctl(loop->efd, EPOLL_CTL_ADD, ioc->fd, &ev); 

	if(err == -1){ 
		if(errno != EEXIST){ /*ignore this one*/
			err = -EIO_LOOP_WATCH;
			perror("epoll_ctl");
			return err;
		}
	}

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
		ioc->status = EV_ERROR;
		close(ioc->fd);
	}
	return OP_SUCCESS;
}



static int loop_retry_io(loop_t * loop){
	assert(loop != NULL && "loop_t pointer loop is NULL");
	int i, err;
	io_core_t * io;
	for(i = 0; i < loop->retry_list.count; i++){
		io = loop->retry_list.arr[i];
		if(io->fd != IO_OFF){
			err = loop_start_io(loop, io);
			if(err < 0) return err;
			printf("NOW WE'RE TALKIN BABY\n");
		}
	}
	return OP_SUCCESS;
}

