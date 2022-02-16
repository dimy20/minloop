#ifndef QUEUE_H
#define QUEUE_H
typedef struct queue_s queue_t;
typedef struct qnode_s qnode_t;

struct qnode_s{
    void * val;
    struct qnode_s * next;
};

struct queue_s {
    qnode_t * head;
    qnode_t * tail;
    unsigned int size;
};

qnode_t * qnode_create(void * val);
void queue_init(queue_t * q);
void queue_insert(queue_t * q, void * val);

unsigned int queue_empty(queue_t * q);


#endif
