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

int _io_accept(io_core_t * io){
	assert(io != NULL && "io_core_t pointer is NULL");
	assert( iocore_getfd(io) >= 0 );

	int err;
	err = accept(iocore_getfd(io), NULL, NULL);
	if(err < 0) return -EIO_ACCEPT;

	return err;
};

int iocore_getfd(const io_core_t * ioc){
	assert(ioc != NULL && "io_core_t pointer is NULL");
	return ioc->fd;
}

void iocore_setfd(io_core_t * ioc, int fd){
	assert(ioc != NULL && "io_core_t pointer is NULL");
	ioc->fd = fd;
}


