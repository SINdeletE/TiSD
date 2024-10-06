#ifndef VECTOR_TOOLS_H__

#define VECTOR_TOOLS_H__

#include <stddef.h>

#define MIN_NUM -10000000
#define MAX_NUM 10000000





#define VEC_INIT_OK 0
#define VEC_INIT_ERR_ALLOC 1

typedef struct 
{
    int *B;
    size_t *JB;

    size_t size;
} vector_t;

void vector_free(vector_t *vector);
int vector_autoinit(vector_t *vector, size_t m, int percent);

#endif
