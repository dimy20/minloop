#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include "../include/vector.h"
#include "../include/error.h"


/*Pass zero as size to use defaults*/
void vector_init(vector_t * vec, size_t size){
	assert(vec != NULL && "vector_t pointer is NULL");
	if(size == 0)
		size = VEC_DEF_SIZE;

	vec->size = size;
	vec->count = 0;

	vec->arr = malloc(sizeof( io_core_t * ) * size);

	error_malloc(vec->arr);

	/*Initialize all pointers to null*/
	for( int i = 0; i < size; i++ ){
		vec->arr[i] = NULL;
	}
}

unsigned int vector_insert(vector_t * vec, int index, io_core_t * ioc){
	assert(vec != NULL && "vector_t pointer is NULL");
	assert(index >= 0 && "Invalid index");
	assert(vec->arr != NULL && "Invalid vector");

	int ret;
	ret = vector_maybe_resize(vec);
	if(ret == 0)
		assert(index < vec->size);
	
	vec->arr[index] = ioc;
	vec->count++;

	return vec->count;
}


unsigned int vector_maybe_resize(vector_t * vec){
	assert(vec != NULL && "vector_t pointer is NULL");

	size_t new_size;

	new_size = vec->count * 2;

	if( new_size > vec->size || vec->size == vec->count){
		vec->arr = realloc(vec->arr, new_size);
		error_malloc(vec->arr);
		return 1;
	}

	return 0;
}

void vector_free(vector_t * vec){
	assert(vec != NULL && "vec is NULL");
	free(vec->arr);
}

