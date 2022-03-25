#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/timer.h"
#include "../include/error.h"



int timer_init(min_timer_t * timer, int repeat){
	assert(timer != NULL && "timer_t pointer is NULL");
	memset(timer, 0, sizeof(min_timer_t));
	if(repeat < 0 ) return -EINVAL;

	timer->cb = NULL;
	timer->timeout_ms = 0;
	timer->clamped_timeout= 0;
	timer->flags = 0;
	timer->r_count = 0;
	timer->repeat = repeat;
	if(timer->repeat > 0)
		timer->flags |= TIMER_REPEAT;
	return OP_SUCCESS;
}

uint64_t timer_get_ms_time(void){
	int ret;
	int err;
	struct timeval tv;

	memset(&tv, 0, sizeof(tv));
	err = gettimeofday(&tv, NULL);
	if(err < 0) return err;
	
	ret = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return ret;
}


int timer_start(loop_t * loop, min_timer_t * timer, int timeout, timer_cb_t cb){
	assert(timer != NULL && "timer_t pointer is NULL");
	assert(loop != NULL && "loop_t pointer is NULL");
	if(cb == NULL) return -EINVAL;
	uint64_t expiry;

	loop->time = timer_get_ms_time();

	expiry = loop->time + timeout;
	if(expiry < timeout)
		expiry = (uint64_t) - 1;

	timer->timeout = expiry;
	timer->cb = cb;

	return heap_insert(&loop->timer_heap, &timer->timeout);
}

int compute_next_timeout(loop_t * loop){
	assert(loop != NULL && "loop_t pointer is NULL");

	int * timeout;
	uint64_t diff = 0;

	timeout = heap_min(&loop->timer_heap);

	if(timeout == NULL) return -1;

	/*timeout expired*/
	if(*timeout <= loop->time) return 0;

	diff = *timeout - loop->time;
	
	return diff > INT_MAX ? INT_MAX : diff;
}
