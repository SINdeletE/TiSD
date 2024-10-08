#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "matrix_tools.h"

void matrix_autoinit_no_col_fill(matrix_t *matrix, size_t str_i, size_t shift);
int matrix_autoinit_alloc_real_mem(matrix_t *matrix);
void matrix_free_without_JA(matrix_t *matrix);

// НЕ СТИРАЕТ SIZE
void matrix_free(matrix_t *matrix)
{
    matrix_free_without_JA(matrix);

    if (matrix->JA != NULL)
        free(matrix->JA);
    
    matrix->JA = NULL;
}

void matrix_free_without_JA(matrix_t *matrix)
{
    if (matrix->A != NULL)
        free(matrix->A);
    
    matrix->A = NULL;

    if (matrix->IA != NULL)
        free(matrix->IA);
    
    matrix->IA = NULL;
}

int matrix_autoinit(matrix_t *matrix, size_t m, size_t n, int percent)
{
    bool col_flag = false;

    size_t size = 0;
    size_t no_col_shift = 0;

    srand(time(NULL));

    if ((matrix->A = malloc(m * n * sizeof(*(matrix->A)))) == NULL)
    {
        matrix_free(matrix);

        return MAT_INIT_ERR_ALLOC;
    }

    if ((matrix->IA = malloc(m * n * sizeof(*(matrix->IA)))) == NULL)
    {
        matrix_free(matrix);

        return MAT_INIT_ERR_ALLOC;
    }

    if ((matrix->JA = calloc(n + 1, sizeof(*(matrix->JA)))) == NULL)
    {
        matrix_free(matrix);

        return MAT_INIT_ERR_ALLOC;
    }

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
            matrix_autoinit_no_col_fill(matrix, i, no_col_shift);

            no_col_shift = 0;
        }

        if (! col_flag)
            no_col_shift++;

        col_flag = false;
    }

    matrix->JA[n] = size;

    if (no_col_shift)
        matrix_autoinit_no_col_fill(matrix, n, no_col_shift);

    matrix->size = size;
    matrix->JA_size = n + 1;

    switch (matrix_autoinit_alloc_real_mem(matrix))
    {
        case MAT_INIT_ERR_FILL:
            matrix_free(matrix);

            return MAT_INIT_ERR_FILL;
        case MAT_INIT_ERR_ALLOC:
            matrix_free(matrix);

            return MAT_INIT_ERR_ALLOC;
    }

    return MAT_INIT_OK;
}

void matrix_autoinit_no_col_fill(matrix_t *matrix, size_t str_i, size_t shift)
{
    for (size_t i = 1; i <= shift; i++)
        matrix->JA[str_i - i] = matrix->JA[str_i];
}

int matrix_autoinit_alloc_real_mem(matrix_t *matrix)
{
    int *A_cpy;
    size_t *IA_cpy;
    size_t *JA_cpy = matrix->JA;

    if (matrix->size == 0)
        return MAT_INIT_ERR_FILL;

    if ((A_cpy = malloc(matrix->size * sizeof(*(matrix->A)))) == NULL)
    {
        matrix_free(matrix);

        return MAT_INIT_ERR_ALLOC;
    }

    if ((IA_cpy = malloc(matrix->size * sizeof(*(matrix->IA)))) == NULL)
    {
        matrix_free(matrix);

        free(A_cpy);
        return MAT_INIT_ERR_ALLOC;
    }

    // -----

    for (size_t i = 0; i < matrix->size; i++)
    {
        A_cpy[i] = matrix->A[i];
        IA_cpy[i] = matrix->IA[i];
    }

    matrix_free_without_JA(matrix);

    matrix->A = A_cpy;
    matrix->IA = IA_cpy;
    matrix->JA = JA_cpy;

    return 0;
}

void matrix_output_sparse(matrix_t *matrix, vector_t *vector)
{
    printf("full_size = %zu x %zu\n", vector->full_size, matrix->JA_size - 1);

    printf(" index |   ");
    for (size_t i = 0; i < matrix->size; i++)
        printf("%-*zu", CELL_SIZE, i);
    printf("\n");

    printf("   A   |   ");
    for (size_t i = 0; i < matrix->size; i++)
        printf("%-*i", CELL_SIZE, matrix->A[i]);
    printf("\n");

    printf("  IA   |   ");
    for (size_t i = 0; i < matrix->size; i++)
        printf("%-*zu", CELL_SIZE, matrix->IA[i]);
    printf("\n");

    printf("  JA   |   ");
    for (size_t i = 0; i < matrix->JA_size; i++)
        printf("%-*zu", CELL_SIZE, matrix->JA[i]);
    printf("\n");
}

void matrix_output_usual(matrix_t *matrix, vector_t *vector)
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
