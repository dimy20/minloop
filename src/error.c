#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../include/error.h"

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
