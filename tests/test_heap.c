#include "acutest.h"
#include "../include/heap.h"
#include "../include/error.h"

#define ELEMS 40923

void test_heap_insert(void){
	heap_t heap;
	int err;
	int sample_arr[ELEMS+1];
	
	int j = 0;
	for(int i = ELEMS; i>=1 ; i--){
		sample_arr[j++] = i;
	}

	err = heap_alloc(&heap);
	TEST_CHECK(err == OP_SUCCESS);
	TEST_CHECK(heap.arr != NULL);

	TEST_CHECK(heap_empty(&heap));
	j = 0;
	for(int i = ELEMS; i >= 1;i--){
		err = heap_insert(&heap, &sample_arr[j++]);
		TEST_CHECK(err >= 0);
	}

	TEST_CHECK(!heap_empty(&heap));
	TEST_CHECK(*heap_min(&heap) == 1);
	TEST_CHECK(heap.count == ELEMS);


	int left, right;
	for(int i = 0; i < (heap.count / 2) - 1; i++){
		left = i * 2 + 1;
		right = left + 1;
		TEST_CHECK(*heap.arr[left] >= *heap.arr[i]);
		TEST_CHECK(*heap.arr[right] >= *heap.arr[i]);
	}

	if(ELEMS % HEAP_DEFAULT_CAPACITY == 0)
		TEST_CHECK(heap_full(&heap));
	else 
		TEST_CHECK(!heap_full(&heap));

	
	int * prev, *curr;
	prev = heap_min(&heap);
	while(heap.count > 0){
		heap_remove(&heap);
		curr = heap_min(&heap);
		if(curr == NULL) break;
		TEST_CHECK_(*prev <= *curr, "%d <= %d ", *prev, *curr);
		prev = curr;
	}
	TEST_CHECK(heap_empty(&heap));

	/*populate again*/
	j = 0;
	for(int i = ELEMS; i >= 1;i--){
		err = heap_insert(&heap, &sample_arr[j++]);
		TEST_CHECK(err >= 0);
	}

	/*make sure heap is referecing the correct alements*/
	int i = ELEMS - 1;
	while(!heap_empty(&heap)){
		TEST_CHECK(&sample_arr[i--] == heap_min(&heap));
		heap_remove(&heap);
	}
	heap_free(&heap);
}

TEST_LIST = {
	{"test_min_heap", test_heap_insert},
	{0}
};
