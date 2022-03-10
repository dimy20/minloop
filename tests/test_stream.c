#include "acutest.h"
#include "../include/error.h"
#include "../include/stream.h"
#include "../include/loop.h"

void on_connection(stream_t * server){
	printf("undefined\n");
}

void test_stream_init(void){
	loop_t loop;
	loop_init(&loop);

	stream_t * server;
	server = stream_new(&loop);

	TEST_ASSERT(server != NULL);
	TEST_ASSERT(stream_server(server, "localhost", "8080") == OP_SUCCESS);
}

void test_stream_listen(void){
	loop_t loop;
	stream_t * server;
	int err;

	loop_init(&loop);
	server = stream_new(&loop);

	stream_server(server, "localhost", "8080");
	err = stream_listen(server, NULL);

	TEST_ASSERT_(err == -EINVAL, "connection callback cant be NULL");
}


TEST_LIST = {
	{"void stream_init(loop_t * loop)", test_stream_init},
	{"int stream_listen(loop_t *, stream_t *, connection_cb)", test_stream_listen},
    {0}
};
