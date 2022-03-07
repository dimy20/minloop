#ifndef CORE_H
#define CORE_H

#include <stdint.h>    


/*events*/
#define EV_ERROR 0x01
#define EV_IN 0x02
#define EV_OUT 0x04
#define EV_CONNECTION 0x08
/*Error codes*/
#define OP_SUCCESS 0x01
#define EIO_BUSY 0x01
#define EIO_ACCEPT 0x02
#define EIO_START 0x04
#define EIO_EPOLL_CTL 0x08
/*Status codes*/
#define IO_OFF -1 /*This means not descriptor has been assigned yet.*/


typedef struct io_core_s io_core_t;

typedef void(*io_core_cb)(io_core_t * ioc, uint8_t status);

struct io_core_s {
    int fd;
    io_core_cb cb;
    uint32_t events;
    uint8_t status;
};


void io_core_init(io_core_t * ioc, int fd, uint32_t events, io_core_cb cb);
int io_core_fd(const io_core_t * ioc);

#endif

