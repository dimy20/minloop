#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../include/error.h"
#include "../include/core.h"

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
	}
	return "Uknown error code";
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
