#include <stdlib.h>
#include "acutest.h"
#include "../include/queue.h"

#define NODES_N 2048
void init_test_values(int * values){
    values = malloc(sizeof(int) * NODES_N);

    for(int i = 0; i< NODES_N;i++){
        values[i] = i;
    }
}

void test_qnode_create(void){
    int * values;
    init_test_values(values);

    for(int i = 0; i< NODES_N; i++){
        TEST_ASSERT(qnode_create(values + i) != NULL);
    }
}

void test_queue_insert(void){
    int * values;
    init_test_values(values);
    queue_t q;
    queue_init(&q);

    for(int i = 0; i< NODES_N; i++){
        TEST_CHECK(queue_insert(&q, values + i) == values + i);
    }

	TEST_ASSERT(q.size == NODES_N);
}

void test_queue_empty(void){
    int x = 1;
    queue_t q;
    queue_init(&q);

    TEST_CHECK(queue_empty(&q) == 1);

    queue_insert(&q, &x);

    TEST_CHECK(queue_empty(&q) == 0);
}

void test_queue_pop(void){
    int * values;
    init_test_values(values);
    queue_t q;
    queue_init(&q);

    for(int i = 0; i< NODES_N; i++){
        queue_insert(&q, values + i);
		TEST_ASSERT(q.size == i + 1);
    }

    for(int i = 0; i < NODES_N; i++){
        TEST_CHECK(qnode_val(queue_pop(&q)) == values + i);
		TEST_ASSERT(q.size == NODES_N - i);
    }

    TEST_ASSERT(q.tail == q.head);
	TEST_ASSERT(q.size == 0);

}

TEST_LIST = {
    {"unsigned int queue_empty(queue_t * queue)", test_queue_empty},
    {"qnode_t * qnode_create(void * val)", test_qnode_create},
    {"void * queue_insert(queue_t * q, void * val)", test_queue_insert},
    {"qnode_t * queue_pop(queue_t * q)", test_queue_pop},
    {0}
};


