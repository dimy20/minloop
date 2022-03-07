#ifndef CORE_H
#define CORE_H

#include <stdint.h>    

#ifndef container_of
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

/*events*/
#define EV_ERROR 0x01
#define EV_IN 0x02
#define EV_OUT 0x04
#define EV_CONNECTION 0x08

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
void iocore_setfd(io_core_t * ioc, int fd);

#endif

