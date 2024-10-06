#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "matrix_tools.h"

void matrix_autoinit_no_col_fill(matrix_t *matrix, size_t str_i, size_t shift);
int matrix_autoinit_alloc_real_mem(matrix_t *matrix);

// НЕ СТИРАЕТ SIZE
void matrix_free(matrix_t *matrix)
{
    if (matrix->A != NULL)
        free(matrix->A);
    
    matrix->A = NULL;

    if (matrix->IA != NULL)
        free(matrix->IA);
    
    matrix->IA = NULL;

    if (matrix->JA != NULL)
        free(matrix->JA);
    
    matrix->JA = NULL;
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

    if ((matrix->JA = calloc(n, sizeof(*(matrix->JA)) + 1)) == NULL)
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
                matrix->A[size] = rand() % (MAX_NUM - MIN_NUM) + MIN_NUM;
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

    matrix->A[m] = size;

    if (no_col_shift)
        matrix_autoinit_no_col_fill(matrix, m, no_col_shift);

    matrix->size = size;

    if (matrix_autoinit_alloc_real_mem(matrix))
    {
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

    matrix_free(matrix);

    matrix->A = A_cpy;
    matrix->IA = IA_cpy;

    return 0;
}
