#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "matrix_tools.h"
#include "general_consts.h"

void sparse_autoinit_no_col_fill(sparse_t *matrix, size_t str_i, size_t shift);
int sparse_autoinit_alloc_real_mem(sparse_t *matrix);
void sparse_free_without_JA(sparse_t *matrix);

// НЕ СТИРАЕТ SIZE
void sparse_free(sparse_t *matrix)
{
    sparse_free_without_JA(matrix);

    if (matrix->JA != NULL)
        free(matrix->JA);
    
    matrix->JA = NULL;
}

void sparse_free_without_JA(sparse_t *matrix)
{
    if (matrix->A != NULL)
        free(matrix->A);
    
    matrix->A = NULL;

    if (matrix->IA != NULL)
        free(matrix->IA);
    
    matrix->IA = NULL;
}

int sparse_autoinit(sparse_t *matrix, size_t m, size_t n, int percent)
{
    bool col_flag = false;

    size_t size = 0;
    size_t no_col_shift = 0;

    srand(time(NULL));

    if ((matrix->A = malloc(m * n * sizeof(*(matrix->A)))) == NULL)
    {
        sparse_free(matrix);

        return MAT_INIT_ERR_ALLOC;
    }

    if ((matrix->IA = malloc(m * n * sizeof(*(matrix->IA)))) == NULL)
    {
        sparse_free(matrix);

        return MAT_INIT_ERR_ALLOC;
    }

    if ((matrix->JA = calloc(n + 1, sizeof(*(matrix->JA)))) == NULL)
    {
        sparse_free(matrix);

        return MAT_INIT_ERR_ALLOC;
    }

    // while (size != (size_t)((double)(n * m) * (double)percent / 100.0) ? (size_t)((double)(n * m) * (double)percent / 100.0) : 1)
    // {
    col_flag = false;

    size = 0;
    no_col_shift = 0;

    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < m; j++)
        {
            if ((rand() % (100 + 1)) <= percent)
            {
                while ((matrix->A[size] = rand() % (MAX_NUM - MIN_NUM) + MIN_NUM) == 0);
                matrix->IA[size] = j;

                if (! col_flag)
                    matrix->JA[i] = size;

                size++;
                col_flag = true;
            }
        }
        
        if (col_flag && no_col_shift)
        {
            sparse_autoinit_no_col_fill(matrix, i, no_col_shift);

            no_col_shift = 0;
        }

        if (! col_flag)
            no_col_shift++;

        col_flag = false;
    }
    // }

    matrix->JA[n] = size;

    if (no_col_shift)
        sparse_autoinit_no_col_fill(matrix, n, no_col_shift);

    matrix->size = size;
    matrix->JA_size = n + 1;

    switch (sparse_autoinit_alloc_real_mem(matrix))
    {
        case MAT_INIT_ERR_FILL:
            sparse_free(matrix);

            return MAT_INIT_ERR_FILL;
        case MAT_INIT_ERR_ALLOC:
            sparse_free(matrix);

            return MAT_INIT_ERR_ALLOC;
    }

    return MAT_INIT_OK;
}

void sparse_autoinit_no_col_fill(sparse_t *matrix, size_t str_i, size_t shift)
{
    for (size_t i = 1; i <= shift; i++)
        matrix->JA[str_i - i] = matrix->JA[str_i];
}

int sparse_autoinit_alloc_real_mem(sparse_t *matrix)
{
    int *A_cpy;
    size_t *IA_cpy;
    size_t *JA_cpy = matrix->JA;

    if (matrix->size == 0)
        return MAT_INIT_ERR_FILL;

    if ((A_cpy = malloc(matrix->size * sizeof(*(matrix->A)))) == NULL)
    {
        sparse_free(matrix);

        return MAT_INIT_ERR_ALLOC;
    }

    if ((IA_cpy = malloc(matrix->size * sizeof(*(matrix->IA)))) == NULL)
    {
        sparse_free(matrix);

        free(A_cpy);
        return MAT_INIT_ERR_ALLOC;
    }

    // -----

    for (size_t i = 0; i < matrix->size; i++)
    {
        A_cpy[i] = matrix->A[i];
        IA_cpy[i] = matrix->IA[i];
    }

    sparse_free_without_JA(matrix);

    matrix->A = A_cpy;
    matrix->IA = IA_cpy;
    matrix->JA = JA_cpy;

    return 0;
}

void sparse_output(sparse_t *matrix, vector_str_t *vector)
{
    size_t str_size = 0;
    size_t i = 0;
    size_t max_i = 0;
    printf("full_size = %zu x %zu\n", vector->full_size, matrix->JA_size - 1);

    while (str_size < (matrix->size > matrix->JA_size ? matrix->size : matrix->JA_size))
    {
        max_i = 0;

        printf(" index |   ");
        i = str_size;
        for (; (i % STR_TABLE_SIZE != 0 || i == str_size) && i < matrix->size;)
            printf("%-*zu", CELL_SIZE, i++);
        max_i = i % STR_TABLE_SIZE > max_i ? i % STR_TABLE_SIZE : max_i;
        if (i - str_size == STR_TABLE_SIZE)
            max_i = STR_TABLE_SIZE;
        printf("\n");

        printf("   A   |   ");
        i = str_size;
        for (; (i % STR_TABLE_SIZE != 0 || i == str_size) && i < matrix->size;)
            printf("%-*i", CELL_SIZE, matrix->A[i++]);
        printf("\n");

        printf("  IA   |   ");
        i = str_size;
        for (; (i % STR_TABLE_SIZE != 0 || i == str_size) && i < matrix->size;)
            printf("%-*zu", CELL_SIZE, matrix->IA[i++]);
        printf("\n");

        printf("  JA   |   ");
        i = str_size;
        for (; (i % STR_TABLE_SIZE != 0 || i == str_size) && i < matrix->JA_size;)
            printf("%-*zu", CELL_SIZE, matrix->JA[i++]);
        max_i = i % STR_TABLE_SIZE > max_i ? i % STR_TABLE_SIZE : max_i;
        if (i - str_size == STR_TABLE_SIZE)
            max_i = STR_TABLE_SIZE;
        printf("\n\n");

        str_size += max_i;
    }
}

void sparse_output_usual(sparse_t *matrix, vector_str_t *vector)
{
    bool flag = false;

    printf("full_size = %zu x %zu\n", vector->full_size, matrix->JA_size - 1);

    for (size_t i = 0; i < vector->full_size; i++)
    {
        for (size_t j = 0; j < matrix->JA_size - 1; j++)
        {
            for (size_t z = matrix->JA[j]; ! flag && z < matrix->JA[j + 1]; z++)
                if (i == matrix->IA[z] && matrix->A[z])
                {
                    printf("%-*d", CELL_SIZE, matrix->A[z]);

                    flag = true;
                }
            
            if (! flag)
                printf("%-*d", CELL_SIZE, 0);

            flag = false;
        }
        printf("\n");
    }
}

// ---------------------------------------------------------------------------------


// НЕ СТИРАЕТ N и M
void matrix_free(matrix_t *matrix)
{
    if (matrix->strs)
    {
        for (size_t i = 0; i < matrix->n; i++)
            free(matrix->strs[i]);

        free(matrix->strs);
    }
    
    matrix->strs = NULL;
}

int matrix_alloc_data(matrix_t *matrix, size_t n, size_t m)
{
    if ((matrix->strs = calloc(n, sizeof(*(matrix->strs)))) == NULL)
        return MAT_ALLOC_ERR;
    
    for (size_t i = 0; i < n; i++)
        if ((matrix->strs[i] = malloc(m * sizeof(int))) == NULL)
        {
            matrix_free(matrix);

            return MAT_ALLOC_ERR;
        }

    return MAT_ALLOC_OK;
}

int sparse_to_matrix(matrix_t *dst, sparse_t *src, vector_str_t *vector)
{
    matrix_free(dst);

    if (matrix_alloc_data(dst, vector->full_size, src->JA_size - 1))
        return MAT_CONVERT_ERR_ALLOC;

    bool flag = false;

    for (size_t i = 0; i < vector->full_size; i++)
    {
        for (size_t j = 0; j < src->JA_size - 1; j++)
        {
            for (size_t z = src->JA[j]; ! flag && z < src->JA[j + 1]; z++)
                if (i == src->IA[z] && src->A[z])
                {
                    dst->strs[i][j] = src->A[z];

                    flag = true;
                }
            
            if (! flag)
                dst->strs[i][j] = 0;

            flag = false;
        }
    }

    dst->n = vector->full_size;
    dst->m = src->JA_size - 1;

    return MAT_CONVERT_OK;
}
