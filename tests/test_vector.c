#include "../include/vector.h"
#include "acutest.h"

#define NO_EVENTS 0
#define ELEM_NO 40

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

void test_vector_init(void){
	size_t size = 0;
	vector_t * vec = create_sample_vector(size);

	TEST_ASSERT_(vec->count == 0, "When initialized count must be zero");

	if(size == 0){
		TEST_ASSERT(vec->size  == VEC_DEF_SIZE);
	}else{
		TEST_ASSERT(vec->size  == size);
	}
	TEST_ASSERT_(vec->arr != NULL, "Failed to allocated memory for vector");
};

void test_vector_insert(void){
	size_t size = 0;
	int fake_fd, ret;

	vector_t * vec = create_sample_vector(size);
	for(int i = 0; i < 40;i++){
		fake_fd = i;
		ret = vector_insert(vec, fake_fd, create_sample_io(fake_fd));
		TEST_ASSERT(ret == (fake_fd + 1));
	}

	TEST_ASSERT(vec->size > vec->count);
}

void test_vector_remove(void){

	size_t size =0 ;
	int fake_fd, ret;

	vector_t * vec = create_sample_vector(size);
	io_core_t  * ios[ELEM_NO];

	/*create 40 io_core_t**/
	for(int i = 0; i < ELEM_NO; i++){
		fake_fd = i;
		ios[i] = create_sample_io(fake_fd);
	}

	for(int i = 0; i < ELEM_NO ;i++){
		fake_fd = i;
		ret = vector_insert(vec, fake_fd, ios[i]);
		TEST_ASSERT(ret == (fake_fd + 1));
	}

	io_core_t * aux;

	for(int  i = 0; i < ELEM_NO ;i++){
		aux = vector_remove(vec,i);
		TEST_ASSERT(aux == ios[i]);
		free(aux);
	}


	TEST_ASSERT(vec->count == 0);


}

/*This test attempts to remove random elemens from the vector*/
void test_vector_remove_random(void){
	srand(time(NULL));
	int fake_fd, ret;
	io_core_t  * ios[ELEM_NO];
	int size = 0;
	vector_t * vec = create_sample_vector(size);

	io_core_t * aux;

	for(int i = 0; i < ELEM_NO; i++){
		fake_fd = i;
		ios[i] = create_sample_io(fake_fd);
	}

	for(int i = 0; i < ELEM_NO ;i++){
		fake_fd = i;
		ret = vector_insert(vec, fake_fd, ios[i]);
		TEST_ASSERT(ret == (fake_fd + 1));
	}

	int random_fd;

	while(vec->count > 0){
		random_fd = rand() % vec->count;
		aux = vector_remove(vec,random_fd); 
		if(aux != NULL){
			TEST_ASSERT(aux == ios[random_fd]);
			free(aux);
		}
	}

	TEST_ASSERT(vec->count == 0);

}

TEST_LIST = {
	{"void vector_init(vector_t * vec, size_t size)", test_vector_init},
	{"unsigned int vector_insert(vector_t * vec, int index, io_core_t *ioc)",
	test_vector_insert},
	{"io_core_t * vector_remove(vector_t * vec, int index)",test_vector_remove},
	{"(random indices )io_core_t * vector_remve(vector_t * vec, int index)", test_vector_remove_random},
	{0}
	
};
