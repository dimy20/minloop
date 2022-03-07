#include "acutest.h"
#include "../include/core.h"
#include "../include/stream.h"
#include "../include/loop.h"

void test_stream_init(void){
	loop_t loop;
	stream_t server;

	loop_init(&loop);
	TEST_ASSERT(stream_init(&loop, &server) == OP_SUCCESS);

	stream_server(&loop, &server, "localhost", "8080");

}

TEST_LIST = {
	{"void stream_init(loop_t * loop)", test_stream_init},
    {0}
};
