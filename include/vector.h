#ifndef VECTOR_H
#define VECTOR_H
#include <stddef.h>
#include "core.h"

#define VEC_DEF_SIZE 32

typedef struct vector_s vector_t;

struct vector_s {
	int size;
	int count;
    io_core_t ** arr;
};

void vector_init(vector_t * vec, size_t size);
void vector_free(vector_t * vec);
unsigned int vector_maybe_resize(vector_t * vec);
int vector_push_back(vector_t * vec, io_core_t * val);
io_core_t * vector_pop_back(vector_t * vec);
int vector_insert(vector_t * vec, int index, io_core_t * ioc);
io_core_t * vector_remove(vector_t * vec, int index);
int vector_empty(const vector_t * vec);


#endif
