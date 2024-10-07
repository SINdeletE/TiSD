#include "operations.h"

int vector_sparse_multiplic(vector_t *res, vector_t *vector, matrix_t *matrix)
{
    if (vector_parameters_assign(res, vector))
    {
        vector_free(res);

        return MULTI_INIT_ERR_ALLOC;
    }

    // Вот эту чухню переделать
    for (size_t i = 0; i < res->size; i++)
        for (size_t j = 0; j < vector->size; j++)
        {
            for (size_t z = matrix->JA[res->JB[j]]; z < matrix->JA[res->JB[j] + 1]; z++)
                if (res->JB[j] == matrix->IA[z])
                {
                    res->B[i] += vector->B[j] * matrix->A[z];

                    break;
                }
        }

    return MULTI_INIT_OK;
}
