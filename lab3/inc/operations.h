#ifndef OPERATIONS_H__

#define OPERATIONS_H__

#include "matrix_tools.h"
#include "vector_tools.h"

#define MULTI_INIT_OK 0
#define MULTI_INIT_ERR_ALLOC 1

#define VEC_ASSIGN_OK 0
#define VEC_ASSIGN_ERR_ALLOC 1

#define VEC_ALLOC_REAL_OK 0
#define VEC_ALLOC_REAL_ERR_ALLOC 1

int vector_str_sparse_multiplic(vector_str_t *res, vector_str_t *vector, sparse_t *matrix);
int vector_matrix_multiplic(vector_t *res, vector_t *vector, matrix_t *matrix);

#endif