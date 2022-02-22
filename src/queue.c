#include <stdlib.h>
#include <assert.h>
#include "../include/queue.h"
#include "../include/error.h"


/*Caller is responsible of allocating and claiming back resources.
 * queue_init does not allocate any memory for queue_t object.*/
void queue_init(queue_t * q){
    assert(q != NULL && "q is NULL");

	memset(q, 0, sizeof(*q));
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

/*free here or at the parent?*/
void * qnode_val(qnode_t * node){
    assert(node != NULL && "node pointer is NULL");
    void * val = node->val;

    free(node);
    return val;
}

void * queue_insert(queue_t * q, void * val){
    assert(q != NULL && "queue_t pointer is NULL");
    qnode_t * node = qnode_create(val);

    if(queue_empty(q)){
        q->tail = q->head = node;
        return node->val;
    }

    q->tail->next = node;
    q->tail = node;
    return node->val;
}



qnode_t * queue_pop(queue_t * q){
    if(queue_empty(q)) return NULL;
    
    qnode_t * aux;
    aux = q->head;

    if(aux == q->tail){
        q->head = q->tail = NULL;
        return aux;
    }

    q->head = q->head->next;
    aux->next = NULL;
    return aux;
}


unsigned int queue_empty(const queue_t * q){
    assert(q != NULL && "q pointer is NULL");
    return (q->tail == NULL && q->head == NULL);
}
