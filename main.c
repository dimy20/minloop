#include <stdio.h>
#include <stdlib.h>
#include "include/vector.h"
#include "include/queue.h"


#define NO_EVENTS 0

io_core_t * create_sample_io(int fd){
	io_core_t * ioc = malloc(sizeof(io_core_t));
	io_core_init(ioc, fd, NO_EVENTS);
	return ioc;
	}

vector_t * create_sample_vector(size_t size){
	vector_t * vec = malloc(sizeof(vector_t));
	vector_init(vec, size);
	return vec;
}
int * init_test_values(){

    int * values = malloc(sizeof(int) * 10);

    for(int i = 0; i< 10 ;i++){
        values[i] = i;
    }
	return values;
}

int main(){
	return 0;
}
