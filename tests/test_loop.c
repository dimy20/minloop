#include "../include/loop.h"
#include "../include/queue.h"
#include "acutest.h"


void test_loop_init(void){
	loop_t loop;
	loop_init(&loop);

	TEST_ASSERT_(loop.efd >= 0, "epoll instance fd is invalid");
	TEST_ASSERT_(loop.pending_q != NULL, "loop's pending queue is NULL");
	TEST_ASSERT_(queue_empty(loop.pending_q), "queue has to be empty at init");
	TEST_ASSERT_(loop.io_watchers == NULL, "io watcher");
	TEST_ASSERT_(loop.poll_fds == NULL, "poll_fds");

	loop_free(&loop);
};

TEST_LIST = {
	{"void loop_init(loop_t * loop)", test_loop_init},
	{0}
};




