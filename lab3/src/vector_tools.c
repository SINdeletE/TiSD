#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "vector_tools.h"
#include "general_consts.h"

int vector_str_autoinit_alloc_real_mem(vector_str_t *vector);

// НЕ СТИРАЕТ SIZE
void vector_str_free(vector_str_t *vector)
{
    if (vector->B != NULL)
        free(vector->B);
    
    vector->B = NULL;

    if (vector->JB != NULL)
        free(vector->JB);
    
    vector->JB = NULL;
}

int vector_str_autoinit(vector_str_t *vector, size_t m, int percent)
{
    size_t size = 0;

    srand(time(NULL));

    if ((vector->B = malloc(m * sizeof(*(vector->B)))) == NULL)
    {
        vector_str_free(vector);

        return VEC_INIT_ERR_ALLOC;
    }

    if ((vector->JB = malloc(m * sizeof(*(vector->JB)))) == NULL)
    {
        vector_str_free(vector);

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

    if (vector_str_autoinit_alloc_real_mem(vector))
    {
        vector_str_free(vector);

        return VEC_INIT_ERR_ALLOC;
    }

    return VEC_INIT_OK;
}

int vector_str_autoinit_alloc_real_mem(vector_str_t *vector)
{
    int *B_cpy;
    size_t *JB_cpy;

    if ((B_cpy = malloc(vector->size * sizeof(*(vector->B)))) == NULL)
    {
        vector_str_free(vector);

        return VEC_INIT_ERR_ALLOC;
    }
    if ((JB_cpy = malloc(vector->size * sizeof(*(vector->JB)))) == NULL)
    {
        vector_str_free(vector);

        free(B_cpy);
        return VEC_INIT_ERR_ALLOC;
    }

    for (size_t i = 0; i < vector->size; i++)
    {
        B_cpy[i] = vector->B[i];
        JB_cpy[i] = vector->JB[i];
    }

    vector_str_free(vector);

    vector->B = B_cpy;
    vector->JB = JB_cpy;

    return 0;
}

void vector_str_output(vector_str_t *vector)
{
    printf("full_size = %zu\n", vector->full_size);

    printf(" index |   ");
    for (size_t i = 0; i < vector->size; i++)
        printf("%-*zu", CELL_SIZE, i);
    printf("\n");

    printf("   B   |   ");
    for (size_t i = 0; i < vector->size; i++)
        printf("%-*d", CELL_SIZE, vector->B[i]);
    printf("\n");

    printf("  JB   |   ");
    for (size_t i = 0; i < vector->size; i++)
        printf("%-*zu", CELL_SIZE, vector->JB[i]);
    printf("\n");
}

void vector_str_output_usual(vector_str_t *vector)
{
    size_t j = 0;
    printf("full_size = %zu\n", vector->full_size);

    for (size_t i = 0; i < vector->full_size; i++)
        if (j < vector->size && vector->JB[j] == i)
        {
            printf("%-*d", CELL_SIZE, vector->B[j]);

            j++;
        }
        else
            printf("%-*d", CELL_SIZE, 0);
}


// ---------------------------------------------------------------------------------


// НЕ СТИРАЕТ SIZE
void vector_free(vector_t *vector)
{
    if (vector->coords != NULL)
        free(vector->coords);
    
    vector->coords = NULL;
}

int vector_str_to_vector(vector_t *dst, vector_str_t *src)
{
    vector_free(dst);

    if ((dst->coords = malloc(src->full_size * sizeof(*(src->B)))) == NULL)
        return VEC_CONVERT_ERR_ALLOC;

    size_t j = 0;
    for (size_t i = 0; i < src->full_size; i++)
        if (j < src->size && src->JB[j] == i)
        {
            dst->coords[i] = src->B[j];

            j++;
        }
        else
            dst->coords[i] = 0;
    
    dst->full_size = src->full_size;

    return VEC_CONVERT_OK;
}
