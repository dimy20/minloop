#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include "../include/timer.h"

void timer_init(timer_t * timer){
	assert(timer != NULL && "timer_t pointer is NULL");
	memset(timer, 0, sizeof(timer_t));

}

uint64_t timet_get_ms_time(void){
	uint64_t ret;
	int err;
	struct timeval tv;

	memset(&tv, 0, sizeof(tv));
	err = gettimeofday(&tv, NULL);
	if(err < 0) return err;
	
	ret = + tv.tv_usec * 1000 + tv.tv_sec / 1000; 
	return ret;
}

