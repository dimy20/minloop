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


int timer_start(loop_t * loop, min_timer_t * timer, int timeout_ms, timer_cb_t cb){
	assert(timer != NULL && "timer_t pointer is NULL");
	assert(loop != NULL && "loop_t pointer is NULL");
	if(cb == NULL || timeout_ms < 0) return -EINVAL;
	/*
	if(timer_is_running(timer))
		return -E_TIMER_RUNNING;
		*/

	timer->timeout_ms = timeout_ms;
	uint64_t expiry;

	loop->time = timer_get_ms_time();

	expiry = loop->time + timeout_ms;
	if(expiry < timeout_ms)
		expiry = (uint64_t) - 1;

	timer->clamped_timeout = expiry;
	timer->cb = cb;

	return heap_insert(&loop->timer_heap, &timer->clamped_timeout);
}

int compute_next_timeout(loop_t * loop){
	assert(loop != NULL && "loop_t pointer is NULL");

	int * clamped_timeout;
	uint64_t diff = 0;

	clamped_timeout = heap_min(&loop->timer_heap);

	if(clamped_timeout == NULL) return -1;

	/*timeout expired*/
	if(*clamped_timeout <= loop->time) return 0;

	diff = *clamped_timeout- loop->time;
	

	return diff > INT_MAX ? INT_MAX : diff;
}

void timer_stop(loop_t * loop, min_timer_t * timer){
	assert(loop != NULL && "loop_t pointer is NULL");
	assert(timer != NULL && "loop_t pointer is NULL");
	int * clamped_timeout;
	clamped_timeout = heap_min(&loop->timer_heap);
	assert(clamped_timeout == &timer->clamped_timeout);
	heap_remove(&loop->timer_heap);
}

int timer_maybe_repeat(loop_t * loop, min_timer_t * timer){
	assert(loop != NULL && "loop_t pointer is NULL");
	assert(timer != NULL && "loop_t pointer is NULL");
	if(timer->flags & TIMER_REPEAT && timer->r_count < timer->repeat){
		timer_start(loop, timer, timer->timeout_ms, timer->cb);
		timer->r_count++;
	}
	return timer->r_count;
}

int timer_is_running(min_timer_t * timer){
	assert(timer != NULL && "loop_t pointer is NULL");
	return timer->flags & TIMER_RUNNING;
}
