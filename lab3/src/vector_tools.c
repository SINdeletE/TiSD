#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "vector_tools.h"

int vector_autoinit_alloc_real_mem(vector_t *vector);

// НЕ СТИРАЕТ SIZE
void vector_free(vector_t *vector)
{
    if (vector->B != NULL)
        free(vector->B);
    
    vector->B = NULL;

    if (vector->JB != NULL)
        free(vector->JB);
    
    vector->JB = NULL;
}

int vector_autoinit(vector_t *vector, size_t m, int percent)
{
    size_t size = 0;

    srand(time(NULL));

    if ((vector->B = malloc(m * sizeof(*(vector->B)))) == NULL)
    {
        vector_free(vector);

        return VEC_INIT_ERR_ALLOC;
    }

    if ((vector->JB = malloc(m * sizeof(*(vector->JB)))) == NULL)
    {
        vector_free(vector);

        return VEC_INIT_ERR_ALLOC;
    }

    for (size_t j = 0; j < m; j++)
    {
        if ((rand() % (100 + 1)) <= percent)
        {
            while ((vector->B[size] = rand() % (MAX_NUM - MIN_NUM) + MIN_NUM) == 0);
            vector->JB[size] = j;

            size++;
        }
    }

    vector->size = size;
    vector->full_size = m;

    if (vector_autoinit_alloc_real_mem(vector))
    {
        vector_free(vector);

        return VEC_INIT_ERR_ALLOC;
    }

    return VEC_INIT_OK;
}

int vector_autoinit_alloc_real_mem(vector_t *vector)
{
    int *B_cpy;
    size_t *JB_cpy;

    if ((B_cpy = malloc(vector->size * sizeof(*(vector->B)))) == NULL)
    {
        vector_free(vector);

        return VEC_INIT_ERR_ALLOC;
    }
    if ((JB_cpy = malloc(vector->size * sizeof(*(vector->JB)))) == NULL)
    {
        vector_free(vector);

        free(B_cpy);
        return VEC_INIT_ERR_ALLOC;
    }

    for (size_t i = 0; i < vector->size; i++)
    {
        B_cpy[i] = vector->B[i];
        JB_cpy[i] = vector->JB[i];
    }

    vector_free(vector);

    vector->B = B_cpy;
    vector->JB = JB_cpy;

    return 0;
}

void vector_str_output(vector_t *vector)
{
    printf("full_size = %zu\n", vector->full_size);

    printf(" index |   ");
    for (size_t i = 0; i < vector->size; i++)
        printf("%-*zu", CELL_SIZE, i);
    printf("\n");

    printf("   B   |   ");
    for (size_t i = 0; i < vector->size; i++)
        printf("%-*i", CELL_SIZE, vector->B[i]);
    printf("\n");

    printf("  JB   |   ");
    for (size_t i = 0; i < vector->size; i++)
        printf("%-*zu", CELL_SIZE, vector->JB[i]);
    printf("\n");
}

int vector_parameters_assign(vector_t *dst, vector_t *src)
{
    if ((dst->B = calloc(src->full_size, sizeof(*(dst->B)))) == NULL)
    {
        vector_free(dst);

        return VEC_ASSIGN_ERR_ALLOC;
    }

    if ((dst->JB = malloc(src->full_size * sizeof(*(dst->JB)))) == NULL)
    {
        vector_free(dst);

        return VEC_ASSIGN_ERR_ALLOC;
    }

    for (size_t i = 0; i < src->size; i++)
        dst->JB[i] = src->JB[i];

    dst->size = src->size;
    dst->full_size = src->full_size;

    return VEC_ASSIGN_OK;
}
