#ifndef CORE_H
#define CORE_H

#include <stdint.h>    

#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

/*status or options*/
#define STS_LISTEN 0x01

#define IOCTL_ADD 1
#define IOCTL_DEL 2

typedef struct loop_s loop_t;

typedef struct io_core_s io_core_t;
typedef void(*io_core_cb)(loop_t * loop, io_core_t * ioc, uint8_t status);

struct io_core_s {
    int fd;
    io_core_cb cb;
    uint32_t events;
    uint8_t status;
};


void io_core_init(io_core_t * ioc, int fd, uint32_t events, io_core_cb cb);
int iocore_getfd(const io_core_t * ioc);
int ioctl(io_core_t * ioc, int op, uint8_t epoll_ev);

#endif

