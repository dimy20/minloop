#include "../include/loop.h"
#include "../include/queue.h"
#include "../include/net.h"
#include "acutest.h"

#define ELEMS 20
#define NOEVENTS 0 

io_core_t * create_sample_io(int fd){
	io_core_t * ioc = malloc(sizeof(io_core_t));
	io_core_init(ioc, fd, NOEVENTS);
	return ioc;
}


void test_loop_init(void){
	loop_t loop;
	loop_init(&loop);

	TEST_ASSERT_(loop.efd >= 0, "epoll instance fd is invalid");
	TEST_ASSERT_(loop.pending_q != NULL, "loop's pending queue is NULL");
	TEST_ASSERT_(queue_empty(loop.pending_q), "queue has to be empty at init");
	//TEST_ASSERT_(loop.io_watchers == NULL, "io watcher");
	TEST_ASSERT_(loop.poll_fds == NULL, "poll_fds");

	loop_free(&loop);
};

void test_loop_start_io(void){
	int ret;
	loop_t loop;
	loop_init(&loop);
	
	io_core_t * vals[ELEMS];

	for(int i = 0; i < ELEMS; i++){
		vals[i]	= create_sample_io(i);
	}

	for(int i = 0; i < ELEMS; i++){
		ret = loop_start_io(&loop, vals[i]);
		TEST_ASSERT(ret == OP_SUCCESS);
	}

	TEST_ASSERT(!queue_empty(loop.pending_q));

}


TEST_LIST = {
	{"void loop_init(loop_t * loop)", test_loop_init},
	{"test",test_loop_start_io},
	{0}
};




