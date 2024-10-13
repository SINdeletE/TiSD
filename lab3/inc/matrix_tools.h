#ifndef MATRIX_TOOLS_H__

#define MATRIX_TOOLS_H__

#include <stddef.h>

#include "vector_tools.h"

#define MAT_INIT_OK 0
#define MAT_INIT_ERR_ALLOC 1
#define MAT_INIT_ERR_FILL 2
#define MAT_INIT_ERR_ZERO_MATRIX 3
#define MAT_INIT_ERR_INVALID_ENTERED_DATA 4

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

    size_t m;
    size_t n;
} matrix_t;

void sparse_free(sparse_t *matrix);
int sparse_to_matrix(matrix_t *dst, sparse_t *src, vector_str_t *vector);
size_t sparse_mem(sparse_t *sparse);

void sparse_output(sparse_t *matrix, vector_str_t *vector);
void sparse_output_usual(sparse_t *matrix, vector_str_t *vector);

// --- 

void matrix_free(matrix_t *matrix);
int matrix_to_sparse(matrix_t *matrix, sparse_t *sparse);

// ---

int matrix_autoinit(matrix_t *matrix, size_t m, size_t n, int percent);
int matrix_init_manual(matrix_t *matrix, size_t m, size_t n);
int matrix_autoinit_by_user(matrix_t *matrix, size_t m, size_t n, int percent);
size_t matrix_mem(matrix_t *matrix);

#endif
