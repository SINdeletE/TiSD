#ifndef VECTOR_TOOLS_H__

#define VECTOR_TOOLS_H__

#include <stddef.h>

#define VEC_INIT_OK 0
#define VEC_INIT_ERR_ALLOC 1
#define VEC_INIT_ERR_FILL 2
#define VEC_INIT_ERR_ZERO_VECTOR 3
#define VEC_INIT_ERR_INVALID_ENTERED_DATA 4

#define VEC_ALLOC_OK 0
#define VEC_ALLOC_ERR 1

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
int vector_str_to_vector(vector_t *dst, vector_str_t *src);

void vector_str_output(vector_str_t *vector);
void vector_str_output_usual(vector_str_t *vector);

int vector_str_parameters_assign(vector_str_t *dst, vector_str_t *src);

size_t vector_str_mem(vector_str_t *vector);

// --- 

void vector_free(vector_t *vector);
int vector_to_vector_str(vector_t *vector, vector_str_t *vector_str);
void vector_output(vector_t *vector);

// ---

int vector_autoinit(vector_t *vector, size_t m, int percent);
int vector_init_manual(vector_t *vector, size_t m);
int vector_autoinit_by_user(vector_t *vector, size_t m, int percent);

size_t vector_mem(vector_t *vector);

#endif
