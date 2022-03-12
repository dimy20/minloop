#include "acutest.h"
#include "../include/error.h"
#include "../include/stream.h"
#include "../include/loop.h"
#include <string.h>
#define BUFF_SIZE 2048
loop_t loop;
void test_stream_init(void){
	loop_init(&loop);

	stream_t * server;
	server = stream_new(&loop);

	TEST_ASSERT(server != NULL);
	TEST_ASSERT(stream_server(server, "localhost", "8080") == OP_SUCCESS);
}

void test_stream_listen(void){

	stream_t * server;
	int err;

	loop_init(&loop);
	server = stream_new(&loop);

	stream_server(server, "localhost", "8080");
	err = stream_listen(server, NULL);

	TEST_ASSERT_(err == -EINVAL, "connection callback cant be NULL");
}

void mock_data(char * dest, int size){
	memset(dest, 0, size);
	char * set = "ABCDEF";
	while(size-- > 0){
		*dest++ = set[rand() % 6-1];
	}
}

void test_stream_send(void){
	char buff[BUFF_SIZE];
	mock_data(buff, BUFF_SIZE);

	loop_init(&loop);
	stream_t * stream = stream_new(&loop);
	TEST_CHECK(stream != NULL);
	TEST_CHECK(stream_write(stream, buff, BUFF_SIZE) == BUFF_SIZE);
	TEST_CHECK(stream_send_ready(stream));
}
TEST_LIST = {
	{"void stream_init(loop_t * loop)", test_stream_init},
	{"int stream_listen(loop_t *, stream_t *, connection_cb)", test_stream_listen},
	{"stream_read", test_stream_send},
    {0}
};

