#include "../include/error.h"
#include <stdlib.h>

void error_exit(int ret, char * msg){
    if(ret == -1){
        perror(msg);
        exit(EXIT_FAILURE);
    }
}
