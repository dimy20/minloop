#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "../include/error.h"
#include "../include/core.h"

#define ERROR_CODE_UNKNOWN "Unknown error code\n";
const char * errstr(int err_code){
	err_code = err_code < 0 ? err_code*-1 : err_code;
	switch(err_code){
		case EIO_START:
			return EIO_START_STR;
		case EIO_BUSY:
			return EIO_BUSY_STR;
		case EIO_LOOP_WATCH:
			return EIO_LOOP_WATCH_STR;
		case EIO_ACCEPT:
			return EIO_ACCEPT_STR;
		case EIO_ACCEPT_LISTEN:
			return EIO_ACCEPT_LISTEN_STR;
		case EIO_INVAL_EV:
			return EIO_INVAL_EV_STR;
		case EIO_BAD_IO:
			return EIO_BAD_IO_STR;
		case E_TIMER_RUNNING:
			return E_TIMER_RUNNING_STR;
	}
	return ERROR_CODE_UNKNOWN;
}

void error_log(char * msg){
    assert(msg != NULL && "msg is NULL");
    fprintf(stderr,"Error : %s \n", msg);
}

void error_exit(int ret, char * msg){
    if(ret == -1){
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

void error_malloc(void * mem){
    if(mem == NULL){
        error_log("Malloc failed to allocate");
        exit(EXIT_FAILURE);
    }
}

int catch_error(int err){
	if(err < 0){
		if(err == -1){
			fprintf(stderr,"Errno: %s \n", strerror(errno));
		}else{
			LOG_ERROR(err);
		}
	}
	return err;
}
