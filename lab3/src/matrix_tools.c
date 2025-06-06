#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "matrix_tools.h"
#include "general_consts.h"

void sparse_autoinit_no_col_fill(sparse_t *matrix, size_t str_i, size_t shift);
int sparse_autoinit_alloc_real_mem(sparse_t *matrix);
void sparse_free_without_JA(sparse_t *matrix);

int matrix_alloc_data(matrix_t *matrix, size_t m, size_t n);

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

int matrix_to_sparse(matrix_t *matrix, sparse_t *sparse)
{
    bool col_flag = false;

    size_t size = 0;
    size_t no_col_shift = 0;

    srand(time(NULL));

    if ((sparse->A = malloc(matrix->m * matrix->n * sizeof(*(sparse->A)))) == NULL)
    {
        sparse_free(sparse);

        return MAT_INIT_ERR_ALLOC;
    }

    if ((sparse->IA = malloc(matrix->m * matrix->n * sizeof(*(sparse->IA)))) == NULL)
    {
        sparse_free(sparse);

        return MAT_INIT_ERR_ALLOC;
    }

    if ((sparse->JA = calloc(matrix->n + 1, sizeof(*(sparse->JA)))) == NULL)
    {
        sparse_free(sparse);

        return MAT_INIT_ERR_ALLOC;
    }

    col_flag = false;

    size = 0;
    no_col_shift = 0;

    for (size_t i = 0; i < matrix->n; i++)
    {
        for (size_t j = 0; j < matrix->m; j++)
            if (matrix->strs[j][i])
            {
                sparse->A[size] = matrix->strs[j][i];
                sparse->IA[size] = j;

                if (! col_flag)
                    sparse->JA[i] = size;

                size++;
                col_flag = true;
            }
        
        if (col_flag && no_col_shift)
        {
            sparse_autoinit_no_col_fill(sparse, i, no_col_shift);

            no_col_shift = 0;
        }

        if (! col_flag)
            no_col_shift++;

        col_flag = false;
    }
    // }

    sparse->JA[matrix->n] = size;

    if (no_col_shift)
        sparse_autoinit_no_col_fill(sparse, matrix->n, no_col_shift);

    sparse->size = size;
    sparse->JA_size = matrix->n + 1;

    switch (sparse_autoinit_alloc_real_mem(sparse))
    {
        case MAT_INIT_ERR_FILL:
            sparse_free(sparse);

            return MAT_INIT_ERR_FILL;
        case MAT_INIT_ERR_ALLOC:
            sparse_free(sparse);

            return MAT_INIT_ERR_ALLOC;
    }

    return MAT_INIT_OK;
}

void sparse_autoinit_no_col_fill(sparse_t *matrix, size_t str_i, size_t shift)
{
    for (size_t i = 1; i <= shift; i++)
        matrix->JA[str_i - i] = matrix->JA[str_i];
}

int sparse_autoinit_alloc_real_mem(sparse_t *sparse)
{
    int *A_cpy;
    size_t *IA_cpy;
    size_t *JA_cpy = sparse->JA;

    if (sparse->size == 0)
        return MAT_INIT_ERR_FILL;

    if ((A_cpy = malloc(sparse->size * sizeof(*(sparse->A)))) == NULL)
    {
        sparse_free(sparse);

        return MAT_INIT_ERR_ALLOC;
    }

    if ((IA_cpy = malloc(sparse->size * sizeof(*(sparse->IA)))) == NULL)
    {
        sparse_free(sparse);

        free(A_cpy);
        return MAT_INIT_ERR_ALLOC;
    }

    // -----

    for (size_t i = 0; i < sparse->size; i++)
    {
        A_cpy[i] = sparse->A[i];
        IA_cpy[i] = sparse->IA[i];
    }

    sparse_free_without_JA(sparse);

    sparse->A = A_cpy;
    sparse->IA = IA_cpy;
    sparse->JA = JA_cpy;

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

size_t sparse_mem(sparse_t *sparse)
{
    return sizeof(*sparse) + sizeof(*(sparse->A)) * sparse->size + sizeof(*(sparse->IA)) * sparse->size + \
    sizeof(*(sparse->JA)) * sparse->JA_size;
}

// ---------------------------------------------------------------------------------


// НЕ СТИРАЕТ N и M
void matrix_free(matrix_t *matrix)
{
    if (matrix->strs)
    {
        for (size_t i = 0; i < matrix->m; i++)
            free(matrix->strs[i]);

        free(matrix->strs);
    }
    
    matrix->strs = NULL;
}

int matrix_alloc_data(matrix_t *matrix, size_t m, size_t n)
{
    if ((matrix->strs = calloc(m, sizeof(*(matrix->strs)))) == NULL)
        return MAT_ALLOC_ERR;
    
    for (size_t i = 0; i < m; i++)
        if ((matrix->strs[i] = calloc(n, sizeof(int))) == NULL)
        {
            matrix_free(matrix);

            return MAT_ALLOC_ERR;
        }

    return MAT_ALLOC_OK;
}

int matrix_autoinit(matrix_t *matrix, size_t m, size_t n, int percent)
{
    size_t elems;

    matrix_free(matrix);

    if (matrix_alloc_data(matrix, m, n))
        return MAT_INIT_ERR_ALLOC;

    elems = (size_t)((double)(n * m) * (double)percent / 100.0) ? (size_t)((double)(n * m) * (double)percent / 100.0) : 1;

    while (elems)
        for (size_t i = 0; elems && i < m; i++)
            for (size_t j = 0; elems && j < n; j++)
                if (!matrix->strs[i][j] && (rand() % (100 + 1)) <= percent)
                {
                    while ((matrix->strs[i][j] = rand() % (MAX_NUM - MIN_NUM) + MIN_NUM) == 0);

                    elems--;
                }
    
    matrix->m = m;
    matrix->n = n;

    return MAT_INIT_OK;
}

int matrix_autoinit_by_user(matrix_t *matrix, size_t m, size_t n, int percent)
{
    size_t elems;

    matrix_free(matrix);

    if (matrix_alloc_data(matrix, m, n))
        return MAT_INIT_ERR_ALLOC;

    elems = (size_t)((double)(n * m) * (double)percent / 100.0) ? (size_t)((double)(n * m) * (double)percent / 100.0) : 1;

    while (elems)
        for (size_t i = 0; elems && i < m; i++)
            for (size_t j = 0; elems && j < n; j++)
                if (!matrix->strs[i][j] && (rand() % (100 + 1)) <= percent)
                {
                    printf("Enter %zu x %zu element (%zu elements remaining): ", i, j, elems);
                    if (scanf("%d", &matrix->strs[i][j]) != 1 || matrix->strs[i][j] == 0)
                    {
                        matrix_free(matrix);
                        return MAT_INIT_ERR_INVALID_ENTERED_DATA;
                    }

                    elems--;
                }
    
    matrix->m = m;
    matrix->n = n;

    return MAT_INIT_OK;
}

int sparse_to_matrix(matrix_t *dst, sparse_t *src, vector_str_t *vector)
{
    matrix_free(dst);

    if (matrix_alloc_data(dst, vector->full_size, src->JA_size - 1))
        return MAT_CONVERT_ERR_ALLOC;

    bool flag = false;

    for (size_t i = 0; i < vector->full_size; i++)
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

    dst->n = vector->full_size;
    dst->m = src->JA_size - 1;

    return MAT_CONVERT_OK;
}

int matrix_init_manual(matrix_t *matrix, size_t m, size_t n)
{
    bool is_zero_matrix = true;

    matrix_free(matrix);

    if (matrix_alloc_data(matrix, m, n))
        return MAT_INIT_ERR_ALLOC;

    for (size_t i = 0; i < m; i++)
        for (size_t j = 0; j < n; j++)
            if (scanf("%d", &matrix->strs[i][j]) != 1)
            {
                matrix_free(matrix);
                return MAT_INIT_ERR_FILL;
            }
            else if (matrix->strs[i][j] != 0)
                is_zero_matrix = false;

    if (is_zero_matrix)
        return MAT_INIT_ERR_ZERO_MATRIX;

    matrix->m = m;
    matrix->n = n;

    return MAT_INIT_OK;
}

int matrix_init_coords(matrix_t *matrix, size_t m, size_t n)
{
    bool is_zero_matrix = true;
    int tmp;
    size_t size_fill = 0;
    size_t ch_i, ch_j;

    matrix_free(matrix);

    if (matrix_alloc_data(matrix, m, n))
        return MAT_INIT_ERR_ALLOC;

    // for (size_t i = 0; i < m; i++)
    //     for (size_t j = 0; j < n; j++)
    while (size_fill <= m * n)
    {
        printf("Enter random int value (if you want to stop, enter -1): ");
        if (scanf("%d", &tmp) != 1)
        {
            matrix_free(matrix);
            return MAT_INIT_ERR_FILL;
        }

        if (tmp == -1)
            break;

        printf("Enter coordinates (entered %zu of %zu) (str and col): ", size_fill, m * n);
        if (scanf("%zu", &ch_i) != 1 && ch_i >= m)
        {
            matrix_free(matrix);
            return MAT_INIT_ERR_FILL;
        }

        if (scanf("%zu", &ch_j) != 1 || ch_j >= n)
        {
            matrix_free(matrix);
            return MAT_INIT_ERR_FILL;
        }

        printf("Enter element value (non zero): ");
        if (scanf("%d", &tmp) != 1 || tmp == 0)
        {
            matrix_free(matrix);
            return MAT_INIT_ERR_FILL;
        }
        else if (tmp != 0)
            is_zero_matrix = false;

        matrix->strs[ch_i][ch_j] = tmp;

        size_fill++;
    }

    if (is_zero_matrix)
        return MAT_INIT_ERR_ZERO_MATRIX;

    matrix->m = m;
    matrix->n = n;

    return MAT_INIT_OK;
}

size_t matrix_mem(matrix_t *matrix)
{
    return sizeof(*matrix) + sizeof(*(matrix->strs)) * matrix->m + \
    sizeof(**(matrix->strs)) * matrix->m * matrix->n;
}
