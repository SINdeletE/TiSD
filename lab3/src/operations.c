#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "operations.h"
#include "general_consts.h"

int vector_str_start_parameters_assign(vector_str_t *dst, size_t size);
int vector_str_sparse_alloc_real_mem(vector_str_t *vector);

int vector_str_sparse_multiplic(vector_str_t *res, vector_str_t *vector, sparse_t *matrix)
{
    vector_str_free(res);

    if (vector_str_start_parameters_assign(res, matrix->JA_size - 1))
    {
        vector_str_free(res);

        return MULTI_INIT_ERR_ALLOC;
    }

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

int vector_str_vector_str_multiplic(vector_str_t *res, vector_str_t *vector, vector_str_t *matrix_vector)
{
    vector_str_free(res);

    if (vector_str_start_parameters_assign(res, matrix_vector->full_size))
    {
        vector_str_free(res);

        return MULTI_INIT_ERR_ALLOC;
    }

    for (size_t i = 0; i < matrix_vector->size; i++)
        res->B[i] += *(vector->B) * matrix_vector->B[i];

    if (vector_str_sparse_alloc_real_mem(res))
    {
        vector_str_free(res);

        return MULTI_INIT_ERR_ALLOC;
    }

    return MULTI_INIT_OK;
}

int vector_str_start_parameters_assign(vector_str_t *dst, size_t size)
{
    if ((dst->B = calloc(size, sizeof(*(dst->B)))) == NULL)
    {
        vector_str_free(dst);

        return VEC_ASSIGN_ERR_ALLOC;
    }

    if ((dst->JB = malloc(size * sizeof(*(dst->JB)))) == NULL)
    {
        vector_str_free(dst);

        return VEC_ASSIGN_ERR_ALLOC;
    }

    for (size_t i = 0; i < size; i++)
        dst->JB[i] = i;

    dst->size = size;
    dst->full_size = size;

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

int vector_start_parameters_assign(vector_t *dst, size_t n)
{
    if ((dst->coords = calloc(n, sizeof(*(dst->coords)))) == NULL)
    {
        vector_free(dst);

        return VEC_ASSIGN_ERR_ALLOC;
    }

    dst->full_size = n;

    return VEC_ASSIGN_OK;
}

int vector_matrix_multiplic(vector_t *res, vector_t *vector, matrix_t *matrix)
{
    vector_free(res);

    if (vector_start_parameters_assign(res, matrix->n))
    {
        vector_free(res);

        return MULTI_INIT_ERR_ALLOC;
    }

    for (size_t j = 0; j < matrix->n; j++)
        for (size_t z = 0; z < matrix->m; z++)
            res->coords[j] += vector->coords[z] * matrix->strs[z][j];

    return MULTI_INIT_OK;
}

int vector_vector_multiplic(vector_t *res, vector_t *vector, vector_t *matrix_vector)
{
    vector_free(res);

    if (vector_start_parameters_assign(res, matrix_vector->full_size))
    {
        vector_free(res);

        return MULTI_INIT_ERR_ALLOC;
    }

    for (size_t j = 0; j < matrix_vector->full_size; j++)
        res->coords[j] += *(vector->coords) * matrix_vector->coords[j];

    return MULTI_INIT_OK;
}

void all_stat_data_free(vector_t *vector, vector_str_t *vector_str, matrix_t *matrix, sparse_t *sparse, vector_str_t *r1, vector_t *r2)
{
    vector_free(vector);
    vector_str_free(vector_str);

    matrix_free(matrix);
    sparse_free(sparse);

    vector_str_free(r1);
    vector_free(r2);
}

void all_stat_data_free_for_vector(vector_t *vector, vector_str_t *vector_str, vector_t *matrix_vector, vector_str_t *matrix_vector_str, vector_str_t *r1, vector_t *r2)
{
    vector_free(vector);
    vector_str_free(vector_str);

    vector_free(matrix_vector);
    vector_str_free(matrix_vector_str);

    vector_str_free(r1);
    vector_free(r2);
}


int vector_matrix_statistics(size_t m, size_t n)
{
    vector_t vector = (vector_t){NULL, 0};
    vector_str_t vector_str = (vector_str_t){NULL, NULL, 0, 0};
    sparse_t sparse = (sparse_t){NULL, NULL, NULL, 0, 0};
    matrix_t matrix = (matrix_t){NULL, 0, 0};

    vector_str_t res_vector_str = (vector_str_t){NULL, NULL, 0, 0};
    vector_t res_vector = (vector_t){NULL, 0};

    struct timespec t_beg, t_end;

    size_t mem_size1 = 0, mem_size2 = 0;
    long time1 = 0, time2 = 0;

    int percent = 1;

    printf("\nSTATISTICS (time in nsec) (memory in bytes)\n");

    printf(" %%percent |");
    printf(" time (str_sparse)  |");
    printf("time (vector_matrix)|");
    printf("  mem (str_sparse)  |");
    printf("mem (vector_matrix) |");
    printf("   smaller memory   |");
    printf("     best time      |\n");

    all_stat_data_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

    while (percent <= 100)
    {
        if (vector_autoinit(&vector, m, percent))
        {
            all_stat_data_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

            return VEC_INIT_ERR_ALLOC;
        }

        if (vector_to_vector_str(&vector, &vector_str))
        {
            all_stat_data_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);
        
            return VEC_CONVERT_ERR_ALLOC;
        }

        if (matrix_autoinit(&matrix, m, n, percent))
        {
            all_stat_data_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

            return MAT_INIT_ERR_ALLOC;
        }

        if (matrix_to_sparse(&matrix, &sparse))
        {
            all_stat_data_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

            return MAT_CONVERT_ERR_ALLOC;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        if (vector_str_sparse_multiplic(&res_vector_str, &vector_str, &sparse))
        {
            all_stat_data_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

            return MULTI_STAT_ERR_ALLOC;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        time1 += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        if (vector_matrix_multiplic(&res_vector, &vector, &matrix))
        {
            all_stat_data_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

            return MULTI_STAT_ERR_ALLOC;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        time2 += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);

        mem_size1 = vector_str_mem(&vector_str) + sparse_mem(&sparse);
        mem_size2 = vector_mem(&vector) + matrix_mem(&matrix);
        
        printf("%-*d|", STR_TABLE_SIZE, percent);
        printf("%-*ld|", STR_TABLE_SIZE * 2, time1);
        printf("%-*ld|", STR_TABLE_SIZE * 2, time2);
        printf("%-*zu|", STR_TABLE_SIZE * 2, mem_size1);
        printf("%-*zu|", STR_TABLE_SIZE * 2, mem_size2);

        if (mem_size1 > mem_size2)
            printf("   vector_matrix    |");
        else if (mem_size1 < mem_size2)
            printf("    str_sparse      |");
        else
            printf("       EQUAL        |");

        if (time1 > time2)
            printf("   vector_matrix    |");
        else if (time1 < time2)
            printf("    str_sparse      |");
        else
            printf("       EQUAL        |");
        
        printf("\n");

        all_stat_data_free(&vector, &vector_str, &matrix, &sparse, &res_vector_str, &res_vector);

        percent++;
    }
    
    return MULTI_STAT_OK;
}

int vector_vector_statistics(size_t m, size_t n)
{
    vector_t vector = (vector_t){NULL, 0};
    vector_str_t vector_str = (vector_str_t){NULL, NULL, 0, 0};
    vector_t matrix_vector = (vector_t){NULL, 0};
    vector_str_t matrix_vector_str = (vector_str_t){NULL, NULL, 0, 0};

    vector_str_t res_vector_str = (vector_str_t){NULL, NULL, 0, 0};
    vector_t res_vector = (vector_t){NULL, 0};

    struct timespec t_beg, t_end;

    size_t mem_size1 = 0, mem_size2 = 0;
    long time1 = 0, time2 = 0;

    int percent = 1;

    printf("\nSTATISTICS (time in nsec) (memory in bytes)\n");

    printf(" %%percent |");
    printf("   time (str_str)   |");
    printf("time (vector_matrix)|");
    printf("    mem (str_str)   |");
    printf("mem (vector_matrix) |");
    printf("   smaller memory   |");
    printf("     best time      |\n");

    all_stat_data_free_for_vector(&vector, &vector_str, &matrix_vector, &matrix_vector_str, &res_vector_str, &res_vector);

    while (percent <= 100)
    {
        if (vector_autoinit(&vector, m, percent))
        {
            all_stat_data_free_for_vector(&vector, &vector_str, &matrix_vector, &matrix_vector_str, &res_vector_str, &res_vector);

            return VEC_INIT_ERR_ALLOC;
        }

        if (vector_to_vector_str(&vector, &vector_str))
        {
            all_stat_data_free_for_vector(&vector, &vector_str, &matrix_vector, &matrix_vector_str, &res_vector_str, &res_vector);
        
            return VEC_CONVERT_ERR_ALLOC;
        }

        if (vector_autoinit(&matrix_vector, n, percent))
        {
            all_stat_data_free_for_vector(&vector, &vector_str, &matrix_vector, &matrix_vector_str, &res_vector_str, &res_vector);

            return VEC_INIT_ERR_ALLOC;
        }

        if (vector_to_vector_str(&matrix_vector, &matrix_vector_str))
        {
            all_stat_data_free_for_vector(&vector, &vector_str, &matrix_vector, &matrix_vector_str, &res_vector_str, &res_vector);
        
            return VEC_CONVERT_ERR_ALLOC;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        if (vector_str_vector_str_multiplic(&res_vector_str, &vector_str, &matrix_vector_str))
        {
            all_stat_data_free_for_vector(&vector, &vector_str, &matrix_vector, &matrix_vector_str, &res_vector_str, &res_vector);

            return MULTI_STAT_ERR_ALLOC;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        time1 += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        if (vector_vector_multiplic(&res_vector, &vector, &matrix_vector))
        {
            all_stat_data_free_for_vector(&vector, &vector_str, &matrix_vector, &matrix_vector_str, &res_vector_str, &res_vector);

            return MULTI_STAT_ERR_ALLOC;
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        time2 += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);

        mem_size1 = vector_str_mem(&vector_str) + vector_str_mem(&matrix_vector_str);
        mem_size2 = vector_mem(&vector) + vector_mem(&matrix_vector);
        
        printf("%-*d|", STR_TABLE_SIZE, percent);
        printf("%-*ld|", STR_TABLE_SIZE * 2, time1);
        printf("%-*ld|", STR_TABLE_SIZE * 2, time2);
        printf("%-*zu|", STR_TABLE_SIZE * 2, mem_size1);
        printf("%-*zu|", STR_TABLE_SIZE * 2, mem_size2);

        if (mem_size1 > mem_size2)
            printf("   vector_matrix    |");
        else if (mem_size1 < mem_size2)
            printf("      str_str       |");
        else
            printf("       EQUAL        |");

        if (time1 > time2)
            printf("   vector_matrix    |");
        else if (time1 < time2)
            printf("      str_str       |");
        else
            printf("       EQUAL        |");
        
        printf("\n");

        all_stat_data_free_for_vector(&vector, &vector_str, &matrix_vector, &matrix_vector_str, &res_vector_str, &res_vector);

        percent++;
    }
    
    return MULTI_STAT_OK;
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
