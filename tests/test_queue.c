#include <stdlib.h>
#include "acutest.h"
#include "../include/queue.h"

#define NODES_N 10

int *  init_test_values(int size){
    int * arr = malloc(sizeof(int) * size);

    for(int i = 0; i< size;i++){
        arr [i] = i;
    }

	return arr;
}

void test_qnode_create(void){
    int * values = init_test_values(NODES_N);

    for(int i = 0; i< NODES_N; i++){
        TEST_ASSERT(qnode_create(values + i) != NULL);
    }
}

void test_queue_nodes( void ){
	int * vals = init_test_values(NODES_N);

	queue_t  * q = malloc(sizeof(queue_t));
	queue_init(q);

	for(int i = 0 ; i < 10 ; i++){
		queue_insert(q, vals + i);
	}

	qnode_t * p;
	p = q->head;
	int i = 0;
	while(p != NULL){
		printf("%p = %p, %d \n", p->val, vals + i, p->val == vals + i);
		TEST_ASSERT(p->val == vals + i);
		if(p->next != NULL){
			printf("%d ->", *(int *)p->val);
		}else{
			printf("%d\n", *(int *)p->val);
		}
		p = p->next;
		i++;
	}
	
}

void test_queue_insert(void){
    int * values = init_test_values(NODES_N);
    queue_t q;
    queue_init(&q);

    for(int i = 0; i< NODES_N; i++){
        TEST_CHECK(queue_insert(&q, values + i) == values + i);
		TEST_ASSERT(q.size == i + 1);
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
    int * values = init_test_values(NODES_N);
   
    queue_t q;
    queue_init(&q);

    for(int i = 0; i< NODES_N; i++){
        queue_insert(&q, values + i);
		TEST_ASSERT(q.size == i + 1);
    }

	TEST_ASSERT(q.size == NODES_N);

    for(int i = 0; i < NODES_N; i++){
        TEST_CHECK(qnode_val(queue_pop(&q)) == values + i);
		TEST_ASSERT(q.size == (NODES_N - (i + 1)));
    }

    TEST_ASSERT(q.tail == q.head);
	TEST_ASSERT(q.size == 0);

}

TEST_LIST = {
    {"unsigned int queue_empty(queue_t * queue)", test_queue_empty},
    {"qnode_t * qnode_create(void * val)", test_qnode_create},
    {"void * queue_insert(queue_t * q, void * val)", test_queue_insert},
    {"qnode_t * queue_pop(queue_t * q)", test_queue_pop},
	{"test_queue_nodes", test_queue_nodes},
    {0}
};


