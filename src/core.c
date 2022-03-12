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
	assert( io->fd >= 0 );

	int err;
	err = accept( io->fd, NULL, NULL);
	if(err < 0) return -EIO_ACCEPT;

	return err;
};

int ioctl(io_core_t * io, int op, uint8_t epoll_ev){
	assert(io != NULL && "io_core_t pointer is NULL");
	int err;
	err = -EIO_INVAL_EV;
	if( op == IOCTL_ADD ){
		io->events |= epoll_ev;
		err = OP_SUCCESS;
	}else if (op == IOCTL_DEL){
		io->events &= ~epoll_ev;
		err = OP_SUCCESS;
	}

	return err;
}

