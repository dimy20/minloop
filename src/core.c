#include <stddef.h>
#include <assert.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../include/core.h"
#include "../include/queue.h"
#include "../include/error.h"



void io_core_init(io_core_t * ioc, int fd, uint32_t events, io_core_cb cb){
    assert(ioc != NULL && "io_core_t * is NULL");
    ioc->fd = fd;
    ioc->events = events;
	ioc->cb = cb;
}

int __io_accept(int server_fd, io_core_t * peer){
	assert(peer != NULL && "io_core_t pointer perr is NULL");
	assert( server_fd >= 0 );
	int accepted;
	accepted = accept(server_fd, NULL, NULL);

	if(accepted == -1)
		return -EIO_ACCEPT;

	peer->fd = accepted;
	peer->cb = NULL;
	peer->events = EPOLLIN | EPOLLET;
	return accepted;
};


int io_core_fd(const io_core_t * ioc){
	assert(ioc != NULL && "io_core_t pointer is NULL");
	return ioc->fd;
}

