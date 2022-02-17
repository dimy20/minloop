#include "../include/vector.h"
#include "acutest.h"


void test_vector_init(void){
	vector_t vec;
	size_t size = 1024;

	vector_init(&vec, size);

	TEST_ASSERT_(vec.count == 0, "When initialized count must be zero");

	if(size == 0){
		TEST_ASSERT(vec.size  == VEC_DEF_SIZE);
	}else{
		TEST_ASSERT(vec.size  == size);
	}
	TEST_ASSERT_(vec.arr != NULL, "Failed to allocated memory for vector");
};


TEST_LIST = {
	{"void vector_init(vector_t * vec, size_t size)", test_vector_init},
	{0}
};
