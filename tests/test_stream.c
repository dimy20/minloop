#include "acutest.h"
#include "../include/error.h"
#include "../include/stream.h"
#include "../include/loop.h"

void on_connection(stream_t * server){
	printf("undefined\n");
}

void test_stream_init(void){
	loop_t loop;
	stream_t server;
	loop_init(&loop);
	TEST_ASSERT(stream_init(&loop, &server) == OP_SUCCESS);
	TEST_ASSERT(stream_server(&server, "localhost", "8080") == OP_SUCCESS);

}

void test_stream_listen(void){
	loop_t loop;
	loop_init(&loop);

	stream_t server;
	int err;

	stream_init(&loop, &server);
	stream_server(&server, "localhost", "8080");

	err = stream_listen(&server, NULL);
	TEST_ASSERT_(err == -EINVAL, "connection callback cant be NULL");
}


TEST_LIST = {
	{"void stream_init(loop_t * loop)", test_stream_init},
	{"int stream_listen(loop_t *, stream_t *, connection_cb)", test_stream_listen},
    {0}
};
