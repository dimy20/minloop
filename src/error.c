#include <stdlib.h>
#include <stdio.h>

#include "../include/error.h"
void error_exit(int ret, char * msg){
    if(ret == -1){
        perror(msg);
        exit(EXIT_FAILURE);
    }
}
