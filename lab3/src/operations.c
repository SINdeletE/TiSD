#include <stdlib.h>

#include "operations.h"

int vector_start_parameters_assign(vector_t *dst, matrix_t *matrix);
int vector_sparse_alloc_real_mem(vector_t *vector);

int vector_sparse_multiplic(vector_t *res, vector_t *vector, matrix_t *matrix)
{
    if (vector_start_parameters_assign(res, matrix))
    {
        vector_free(res);

        return MULTI_INIT_ERR_ALLOC;
    }

    // Вот эту чухню переделать
    for (size_t i = 0; i < res->size; i++)
        for (size_t j = 0; j < vector->size; j++)
            for (size_t z = matrix->JA[res->JB[i]]; z < matrix->JA[res->JB[i] + 1]; z++)
                if (vector->JB[j] == matrix->IA[z])
                {
                    res->B[i] += vector->B[j] * matrix->A[z];

                    break;
                }

    if (vector_sparse_alloc_real_mem(res))
    {
        vector_free(res);

        return MULTI_INIT_ERR_ALLOC;
    }

    return MULTI_INIT_OK;
}

int vector_start_parameters_assign(vector_t *dst, matrix_t *matrix)
{
    if ((dst->B = calloc(matrix->JA_size - 1, sizeof(*(dst->B)))) == NULL)
    {
        vector_free(dst);

        return VEC_ASSIGN_ERR_ALLOC;
    }

    if ((dst->JB = malloc((matrix->JA_size - 1) * sizeof(*(dst->JB)))) == NULL)
    {
        vector_free(dst);

        return VEC_ASSIGN_ERR_ALLOC;
    }

    for (size_t i = 0; i < matrix->JA_size - 1; i++)
        dst->JB[i] = i;

    dst->size = matrix->JA_size - 1;
    dst->full_size = matrix->JA_size - 1;

    return VEC_ASSIGN_OK;
}

int vector_sparse_alloc_real_mem(vector_t *vector)
{
    int *B_cpy;
    size_t *JB_cpy;

    size_t real_size = 0;

    for (size_t i = 0; i < vector->size; i++)
        if (vector->B[i] != 0)
            real_size++;

    vector->size = real_size;

    if ((B_cpy = calloc(real_size, sizeof(*(vector->B)))) == NULL)
    {
        vector_free(vector);

        return VEC_ALLOC_REAL_ERR_ALLOC;
    }

    if ((JB_cpy = malloc(real_size * sizeof(*(vector->JB)))) == NULL)
    {
        vector_free(vector);

        return VEC_ALLOC_REAL_ERR_ALLOC;
    }

    real_size = 0;
    for (size_t i = 0; i < vector->size; i++)
        if (vector->B[i] != 0)
        {
            B_cpy[real_size] = vector->B[i];
            JB_cpy[real_size] = vector->JB[i];

            real_size++;
        }

    vector_free(vector);

    vector->B = B_cpy;
    vector->JB = JB_cpy;

    return VEC_ALLOC_REAL_OK;
}
