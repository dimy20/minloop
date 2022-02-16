#include <stdio.h>
#include "include/stream.h"
#include "include/loop.h"
#include "include/queue.h"
int main(){
    queue_t q;
    queue_init(&q);
    int test[5] = {1,2,3,4,5};


    for(int i = 0; i <5; i++){
        queue_insert(&q, test + i);
    }

    qnode_t * p, * last;

    p = q.head;
    while(p != NULL){
        printf("%d ", *(int *)p->val);
        fflush(stdout);
        p = p->next;
    }
    printf("\n");


    last = queue_pop(&q);
    fprintf(stderr,"Popped: %d\n", *(int *)qnode_val(last));

    last = queue_pop(&q);
    fprintf(stderr,"Popped: %d\n", *(int *)qnode_val(last));


    last = queue_pop(&q);
    fprintf(stderr,"Popped: %d\n", *(int *)qnode_val(last));

    for(int i = 0; i <3; i++){
        printf(" inserting %d", *(test + i));
        fflush(stdout);
        queue_insert(&q, test + i);
    }

    last = queue_pop(&q);
    fprintf(stderr,"Popped: %d\n", *(int *)qnode_val(last));

    last = queue_pop(&q);
    fprintf(stderr,"Popped: %d\n", *(int *)qnode_val(last));



    for(int i = 0; i <3; i++){
        queue_insert(&q, test + i);
    }

    p = q.head;

    while(p != NULL){
        printf("%d ", *(int *)p->val);
        fflush(stdout);
        p = p->next;
    }
    printf("\n");

}
