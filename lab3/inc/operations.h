#ifndef OPERATIONS_H__

#define OPERATIONS_H__

#include "matrix_tools.h"
#include "vector_tools.h"

#define MULTI_INIT_OK 0
#define MULTI_INIT_ERR_ALLOC 1

int vector_sparse_multiplic(vector_t *res, vector_t *vector, matrix_t *matrix);

#endif