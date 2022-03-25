#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>

#include "../include/loop.h"

#define TIMER_RUNNING 0x01
#define TIMER_REPEAT  0x02

typedef void(*timer_cb_t)(void);
typedef struct timer_s min_timer_t;

struct timer_s{
	timer_cb_t cb;
	int clamped_timeout; 
	int timeout_ms;
	int repeat;
	uint8_t flags;
	int r_count;
};


int timer_init(min_timer_t * timer, int repeat);
uint64_t timer_get_ms_time(void);
int timer_start(loop_t * loop, min_timer_t * timer, int timeout_ms, timer_cb_t cb);
int compute_next_timeout(loop_t * loop);
void timer_stop(loop_t * loop, min_timer_t * timer);
int timer_maybe_repeat(loop_t * loop, min_timer_t * timer);
int timer_is_running(min_timer_t * timer);

#endif

