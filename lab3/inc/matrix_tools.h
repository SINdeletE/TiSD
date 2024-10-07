#ifndef MATRIX_TOOLS_H__

#define MATRIX_TOOLS_H__

#include <stddef.h>

#include "general_consts.h"
#include "vector_tools.h"

#define MAT_INIT_OK 0
#define MAT_INIT_ERR_ALLOC 1
#define MAT_INIT_ERR_FILL 2

typedef struct 
{
    int *A;
    size_t *IA;
    size_t *JA;

    size_t size;
    size_t JA_size;
} matrix_t;

void matrix_free(matrix_t *matrix);
int matrix_autoinit(matrix_t *matrix, size_t m, size_t n, int percent);
void matrix_output_sparse(matrix_t *matrix, vector_t *vector);

#endif
