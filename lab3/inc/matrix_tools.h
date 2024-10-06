#ifndef MATRIX_TOOLS_H__

#define MATRIX_TOOLS_H__

#include <stddef.h>

#define MIN_NUM -10000000
#define MAX_NUM 10000000



#define MAT_INIT_OK 0
#define MAT_INIT_ERR_ALLOC 1

typedef struct 
{
    int *A;
    size_t *IA;
    size_t *JA;

    size_t size;
} matrix_t;

void matrix_free(matrix_t *matrix);
int matrix_autoinit(matrix_t *matrix, size_t m, size_t n, int percent);

#endif
