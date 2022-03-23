#ifndef HEAP_H
#define HEAP_H

typedef struct heap_s heap_t;

struct heap_s{
	int * arr;
	int count;
};


int heap_alloc(heap_t * heap);
int heap_insert(heap_t * heap, int val);
void heap_free(heap_t * heap);
int heap_min(const heap_t * heap);
void heap_remove(heap_t * heap);
#endif
