#ifndef HEAP_H
#define HEAP_H

#define HEAP_DEFAULT_CAPACITY 512

typedef struct heap_s heap_t;

struct heap_s{
	int ** arr;
	size_t count;
	size_t capacity;
};


int heap_alloc(heap_t * heap);
int heap_insert(heap_t * heap, int * val);
void heap_free(heap_t * heap);
int * heap_min(const heap_t * heap);
void heap_remove(heap_t * heap);
int heap_empty(const heap_t * heap);
int heap_full(const heap_t * heap);

#endif
