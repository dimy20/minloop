#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <limits.h>
#include "../include/timer.h"

void timer_init(min_timer_t * timer){
	assert(timer != NULL && "timer_t pointer is NULL");
	memset(timer, 0, sizeof(min_timer_t));
	timer->cb = NULL;
	timer->timeout = 0;
	timer->repeat = 0;
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

