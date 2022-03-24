#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>

#include "../include/loop.h"
typedef void(*timer_cb_t)(void);
typedef struct timer_s min_timer_t;

struct timer_s{
	timer_cb_t cb;
	int timeout;
	int repeat;
};


void timer_init(timer_t * timer);
uint64_t timet_get_ms_time(void);

#endif
