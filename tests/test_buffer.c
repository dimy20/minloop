#include "acutest.h"
#include "../include/qc_buffer.h"
#include "../include/error.h"
#define CHUNK_SIZE 1024

void test_buffer_init(void){
	int err;
	qc_buffer_t buff;
	err = buffer_init(&buff);
	TEST_CHECK(err == OP_SUCCESS);
}
void populate_buffer(char * buff, int j, int size){
	char charset[5] = "ABCDE";
	for(int i = j; i < size; i++){
		buff[i] = charset[(rand() % 5)];
	}
}

void test_buffer_maybe_resize(void){
	int diff, err, count;
	qc_buffer_t buff;
	buffer_init(&buff);
	TEST_CHECK(buffer_empty(&buff));

	count = 0; 
	for(int i = 0; i < 10000; i++){
		err = buffer_maybe_resize(&buff, CHUNK_SIZE);
		TEST_CHECK(err != -1);
		TEST_ASSERT(buff.end + CHUNK_SIZE < buff.capacity);
		buff.end += CHUNK_SIZE;
		if(err > 0) count++;
	}
	TEST_CHECK(count > 0);
	TEST_CHECK(buff.end < buff.capacity);
}

TEST_LIST = {
	{"int buffer_init(qc_buffer_t *, int)", test_buffer_init},
	{"int buffer_maybe_resize(qc_buffer_t *)", test_buffer_maybe_resize},
	{0}
};
