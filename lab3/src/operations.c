#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "operations.h"

int vector_str_start_parameters_assign(vector_str_t *dst, sparse_t *matrix);
int vector_str_sparse_alloc_real_mem(vector_str_t *vector);

int vector_str_sparse_multiplic(vector_str_t *res, vector_str_t *vector, sparse_t *matrix)
{
    vector_str_free(res);

    if (vector_str_start_parameters_assign(res, matrix))
    {
        vector_str_free(res);

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

    if (vector_str_sparse_alloc_real_mem(res))
    {
        vector_str_free(res);

        return MULTI_INIT_ERR_ALLOC;
    }

    return MULTI_INIT_OK;
}

int vector_str_start_parameters_assign(vector_str_t *dst, sparse_t *matrix)
{
    if ((dst->B = calloc(matrix->JA_size - 1, sizeof(*(dst->B)))) == NULL)
    {
        vector_str_free(dst);

        return VEC_ASSIGN_ERR_ALLOC;
    }

    if ((dst->JB = malloc((matrix->JA_size - 1) * sizeof(*(dst->JB)))) == NULL)
    {
        vector_str_free(dst);

        return VEC_ASSIGN_ERR_ALLOC;
    }

    for (size_t i = 0; i < matrix->JA_size - 1; i++)
        dst->JB[i] = i;

    dst->size = matrix->JA_size - 1;
    dst->full_size = matrix->JA_size - 1;

    return VEC_ASSIGN_OK;
}

int vector_str_sparse_alloc_real_mem(vector_str_t *vector)
{
    int *B_cpy;
    size_t *JB_cpy;

    size_t real_size = 0;

    for (size_t i = 0; i < vector->size; i++)
        if (vector->B[i] != 0)
            real_size++;

    if ((B_cpy = calloc(real_size, sizeof(*(vector->B)))) == NULL)
    {
        vector_str_free(vector);

        return VEC_ALLOC_REAL_ERR_ALLOC;
    }

    if ((JB_cpy = malloc(real_size * sizeof(*(vector->JB)))) == NULL)
    {
        vector_str_free(vector);

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

    vector_str_free(vector);

    vector->B = B_cpy;
    vector->JB = JB_cpy;
    vector->size = real_size;

    return VEC_ALLOC_REAL_OK;
}

int vector_start_parameters_assign(vector_t *dst, matrix_t *matrix)
{
    if ((dst->coords = calloc(matrix->n, sizeof(*(dst->coords)))) == NULL)
    {
        vector_free(dst);

        return VEC_ASSIGN_ERR_ALLOC;
    }

    dst->full_size = matrix->n;

    return VEC_ASSIGN_OK;
}

int vector_matrix_multiplic(vector_t *res, vector_t *vector, matrix_t *matrix)
{
    vector_free(res);

    if (vector_start_parameters_assign(res, matrix))
    {
        vector_free(res);

        return MULTI_INIT_ERR_ALLOC;
    }

    for (size_t j = 0; j < matrix->n; j++)
        for (size_t z = 0; z < matrix->m; z++)
            res->coords[j] += vector->coords[z] * matrix->strs[z][j];

    return MULTI_INIT_OK;
}

void all_free(vector_t *vector, vector_str_t *vector_str, matrix_t *matrix, sparse_t *sparse, vector_str_t *r1, vector_t *r2)
{
    vector_free(vector);
    vector_str_free(vector_str);

    matrix_free(matrix);
    sparse_free(sparse);

    vector_str_free(r1);
    vector_free(r2);
}

int vector_matrix_statistics(size_t m, size_t n)
{
    vector_t vector = (vector_t){NULL, 0};
    vector_str_t vector_str = (vector_str_t){NULL, NULL, 0, 0};
    matrix_t matrix;
    sparse_t sparse;

    vector_str_t res_vector_str = (vector_str_t){NULL, NULL, 0, 0};
    vector_t res_vector = (vector_t){NULL, 0};

    struct timespec t_beg, t_end;

    size_t mem_size1 = 0, mem_size2 = 0;
    long time1 = 0, time2 = 0;

    int percent = 1;

    while (percent <= 100)
    {
        all_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

        if (vector_autoinit(&vector, m, percent))
        {
            all_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

            return VEC_INIT_ERR_ALLOC;
        }

        if (vector_to_vector_str(&vector, &vector_str))
        {
            all_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

            return VEC_CONVERT_ERR_ALLOC;
        }

        if (matrix_autoinit(&matrix, m, n, percent))
        {
            all_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

            return MAT_INIT_ERR_ALLOC;
        }

        if (matrix_to_sparse(&matrix, &sparse))
        {
            all_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

            return MAT_CONVERT_ERR_ALLOC;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        if (vector_str_sparse_multiplic(&res_vector_str, &vector_str, &sparse))
        {
            all_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

            return MULTI_STAT_ERR_ALLOC;
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        time1 += 1000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec) / 1000;

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        if (vector_matrix_multiplic(&res_vector, &vector, &matrix))
        {
            all_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

            return MULTI_STAT_ERR_ALLOC;
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        time2 += 1000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec) / 1000;
    }
    
}
