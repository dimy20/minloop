#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
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

int vector_insert(vector_t * vec, int index, io_core_t * ioc){
	assert(vec != NULL && "vector_t pointer is NULL");
	assert(index >= 0 && "Invalid index");
	assert(vec->arr != NULL && "Invalid vector");

	int ret;
	ret = vector_maybe_resize(vec);
	if(ret == 0)
		assert(index < vec->size);

	if( vec->arr[index] != NULL ){
		return -EIO_BUSY;
	}

	vec->arr[index] = ioc;
	vec->count++;

	return vec->count;
}


unsigned int vector_maybe_resize(vector_t * vec){
	assert(vec != NULL && "vector_t pointer is NULL");

	size_t new_size, diff;

	assert(vec->count <= vec->size && "Element count should nevet exceed size");

	if( vec->count + 1 == vec->size ){
		new_size = vec->size * 2;
		vec->arr = realloc(vec->arr, new_size * sizeof(io_core_t *));
		diff = new_size - vec->size;

		/*Initialize all new pointers to NULL*/
		memset(vec->arr + vec->size, 0, diff * sizeof(io_core_t *));
		vec->size += new_size;
		return 1;
	}

	return 0;
}

/*Freeing the memory that is being held by each pointer
 * in the vector's array is the responsibility of the caller.
 * vector_free will just release the size-dynamically-allocated pointers,
 * So if the caller doesnt free any io_core_t's the migth be left in the
 * vector before calling vector_free, they will definitely be lost.
 * The same applies to the vec pointer.*/

void vector_free(vector_t * vec){
	assert(vec != NULL && "vec is NULL");
	free(vec->arr);
}

