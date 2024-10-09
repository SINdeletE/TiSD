#ifndef VECTOR_TOOLS_H__

#define VECTOR_TOOLS_H__

#include <stddef.h>

#define VEC_INIT_OK 0
#define VEC_INIT_ERR_ALLOC 1
#define VEC_INIT_ERR_FILL 2

#define VEC_CONVERT_OK 0
#define VEC_CONVERT_ERR_ALLOC 1

typedef struct 
{
    int *B;
    size_t *JB;

    size_t size;
    size_t full_size;
} vector_str_t;

typedef struct
{
    int *coords;

    size_t full_size;
} vector_t;

void vector_str_free(vector_str_t *vector);
int vector_str_autoinit(vector_str_t *vector, size_t m, int percent);

void vector_str_output(vector_str_t *vector);
void vector_str_output_usual(vector_str_t *vector);

int vector_str_parameters_assign(vector_str_t *dst, vector_str_t *src);

// --- 

void vector_free(vector_t *vector);
int vector_str_to_vector(vector_t *dst, vector_str_t *src);

#endif
