#ifndef MATRIX_TOOLS_H__

#define MATRIX_TOOLS_H__

#include <stddef.h>

#include "vector_tools.h"

#define MAT_INIT_OK 0
#define MAT_INIT_ERR_ALLOC 1
#define MAT_INIT_ERR_FILL 2

#define MAT_CONVERT_OK 0
#define MAT_CONVERT_ERR_ALLOC 1

#define MAT_ALLOC_OK 0
#define MAT_ALLOC_ERR 1

typedef struct 
{
    int *A;
    size_t *IA;
    size_t *JA;

    size_t size;
    size_t JA_size;
} sparse_t;

typedef struct
{
    int **strs;

    size_t n;
    size_t m;
} matrix_t;

void sparse_free(sparse_t *matrix);
int sparse_autoinit(sparse_t *matrix, size_t m, size_t n, int percent);

void sparse_output(sparse_t *matrix, vector_str_t *vector);
void sparse_output_usual(sparse_t *matrix, vector_str_t *vector);

// --- 

void matrix_free(matrix_t *matrix);
int sparse_to_matrix(matrix_t *dst, sparse_t *src, vector_str_t *vector);

#endif
