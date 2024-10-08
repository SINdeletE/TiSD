#ifndef VECTOR_TOOLS_H__

#define VECTOR_TOOLS_H__

#include <stddef.h>

#include "general_consts.h"

#define VEC_INIT_OK 0
#define VEC_INIT_ERR_ALLOC 1
#define VEC_INIT_ERR_FILL 2

typedef struct 
{
    int *B;
    size_t *JB;

    size_t size;
    size_t full_size;
} vector_t;

void vector_free(vector_t *vector);
int vector_autoinit(vector_t *vector, size_t m, int percent);

void vector_str_output(vector_t *vector);
void vector_output_usual(vector_t *vector);

int vector_parameters_assign(vector_t *dst, vector_t *src);

#endif
