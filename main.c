#include <stdio.h>
#include <stdlib.h>
#include "include/vector.h"


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

int main(){

	size_t size = 0;
	int fake_fd, ret;

	vector_t * vec = create_sample_vector(size);


	for(int i = 0; i < 43 ;i++){
		fake_fd = i;
		ret = vector_insert(vec, fake_fd, create_sample_io(fake_fd));
	}

}
