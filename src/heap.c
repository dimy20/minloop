#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include "../include/heap.h"
#include "../include/error.h"

int heap_alloc(heap_t * heap){
	assert(heap != NULL && "heap_t pointer is NULL");
	heap->capacity = HEAP_DEFAULT_CAPACITY;
	heap->arr = malloc(sizeof(int *) * heap->capacity);
	if(heap->arr == NULL) return -EALLOC;
	heap->count = 0;
	return OP_SUCCESS;
}

static void swap(int i , int j, int ** arr){
	assert(i>=0 && j>= 0 && arr != NULL);
	int * tmp;
	tmp = arr[i];
	arr[i] = arr[j];
	arr[j] = tmp;
}
static int heap_maybe_resize(heap_t * heap){
	assert(heap != NULL && "heap_t pointer is NULL");
	size_t new_cap;
	if(heap->count >= heap->capacity){
		new_cap = heap->capacity * 2;
		heap->arr = realloc(heap->arr, new_cap * sizeof(int *));
		if(heap->arr == NULL) return -EALLOC;
		heap->capacity = new_cap;
	}
	return OP_SUCCESS;
}
static void heapify_up(int ** arr, int c_index){
	assert(arr != NULL && "int pointer is NULL");
	while(c_index >= 0 && *arr[(c_index - 1) / 2] > *arr[c_index]){
		swap((c_index - 1) / 2, c_index, arr);
		c_index = (c_index - 1) / 2;
	}
}
int heap_insert(heap_t * heap, int * val){
	assert(heap != NULL && "heap_t pointer is NULL");
	int err;
	err = heap_maybe_resize(heap);
	if(err < 0) return err;
	heap->arr[heap->count] = val;
	heapify_up(heap->arr, heap->count++);
	return OP_SUCCESS;
}
void heapify_top_down(int ** arr,  int n){
	assert(arr != NULL && "int pointer is NULL");
	int i, j;

	i = 0;
	j = i * 2 + 1;
	while( j < n - 1){
		if(*arr[j+1] < *arr[j])
			j = j+1;

		if(*arr[i] >  *arr[j]){
			swap(i, j, arr);
			i = j;
			j = j * 2 + 1;
		}else{
			break;
		}
	}
}
void heap_remove(heap_t * heap){
	assert(heap != NULL && "heap_t pointer is NULL");
	if(heap->count == 0) return;
	heap->arr[0] = heap->arr[heap->count-1];
	heapify_top_down(heap->arr, heap->count);
	heap->count--;
}

int heap_empty(const heap_t * heap){
	assert(heap != NULL && "heap_t pointer is NULL");
	return heap->count == 0;
}

int heap_full(const heap_t * heap){
	assert(heap != NULL && "heap_t pointer is NULL");
	return heap->count == heap->capacity;
}
int * heap_min(const heap_t * heap){
	assert(heap != NULL && "heap_t pointer is NULL");
	if(heap->count == 0) return NULL;
	return heap->arr[0];
}

void heap_free(heap_t * heap){
	assert(heap != NULL && "heap_t pointer is NULL");
	free(heap->arr);
}
