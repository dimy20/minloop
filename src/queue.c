#include <stdlib.h>
#include <assert.h>
#include "../include/queue.h"
#include "../include/error.h"



void queue_init(queue_t * q){
    assert(q != NULL && "q is NULL");
    q->size = 0;
    q->head = q->tail = NULL;
}

qnode_t * qnode_create(void * val){
    qnode_t * node = malloc(sizeof(qnode_t));
    error_malloc(node);

    node->val = val;
    node->next = NULL;
    return node;
}

void queue_insert(queue_t * q, void * val){
    assert(q != NULL && "queue_t pointer is NULL");
    qnode_t * node = qnode_create(val);

    if(queue_empty(q)){
        q->tail = q->head = node;
        return;
    }

    q->tail->next = node;
}

unsigned int queue_empty(queue_t * q){
    assert(q != NULL && "q pointer is NULL");
    return (q->tail == NULL && q->head == NULL);
}
