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

int vector_str_alloc_data(vector_str_t *vector, size_t size)
{
    if ((vector->B = malloc(size * sizeof(*(vector->B)))) == NULL)
    {
        vector_str_free(vector);

        return VEC_ALLOC_ERR;
    }

    if ((vector->JB = malloc(size * sizeof(*(vector->JB)))) == NULL)
    {
        vector_str_free(vector);

        return VEC_ALLOC_ERR;
    }

    return VEC_ALLOC_OK;
}

int vector_to_vector_str(vector_t *vector, vector_str_t *vector_str)
{
    size_t size = 0;

    srand(time(NULL));

    vector_str_free(vector_str);

    vector_str_alloc_data(vector_str, vector->full_size);

    for (size_t j = 0; j < vector->full_size; j++)
    {
        if (vector->coords[j])
        {
            vector_str->B[size] = vector->coords[j];
            vector_str->JB[size] = j;

            size++;
        }
    }

    vector_str->size = size;
    vector_str->full_size = vector->full_size;

    if (vector_str_autoinit_alloc_real_mem(vector_str))
    {
        vector_str_free(vector_str);

        return VEC_CONVERT_ERR_ALLOC;
    }

    return VEC_CONVERT_OK;
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
    size_t str_size = 0;
    size_t i = 0;
    size_t max_i = 0;
    printf("full_size = %zu\n", vector->full_size);

    while (str_size < vector->size)
    {
        max_i = 0;

        printf(" index |   ");
        i = str_size;
        for (; (i % STR_TABLE_SIZE != 0 || i == str_size) && i < vector->size;)
            printf("%-*zu", CELL_SIZE, i++);
        max_i = i % STR_TABLE_SIZE > max_i ? i % STR_TABLE_SIZE : max_i;
        if (i - str_size == STR_TABLE_SIZE)
            max_i = STR_TABLE_SIZE;
        printf("\n");

        printf("   B   |   ");
        i = str_size;
        for (; (i % STR_TABLE_SIZE != 0 || i == str_size) && i < vector->size;)
            printf("%-*d", CELL_SIZE, vector->B[i++]);
        printf("\n");

        printf("  JB   |   ");
        i = str_size;
        for (; (i % STR_TABLE_SIZE != 0 || i == str_size) && i < vector->size;)
            printf("%-*zu", CELL_SIZE, vector->JB[i++]);
        printf("\n\n");

        str_size += max_i;
    }
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
    
    printf("\n");
}

size_t vector_str_mem(vector_str_t *vector)
{
    return sizeof(*vector) + sizeof(*(vector->B)) * vector->size + \
    sizeof(*(vector->JB)) * vector->size;
}


// ---------------------------------------------------------------------------------


// НЕ СТИРАЕТ SIZE
void vector_free(vector_t *vector)
{
    if (vector->coords != NULL)
        free(vector->coords);
    
    vector->coords = NULL;
}

int vector_alloc_data(vector_t *vector, size_t full_size)
{
    if ((vector->coords = calloc(full_size, sizeof(*(vector->coords)))) == NULL)
        return VEC_ALLOC_ERR;

    vector->full_size = full_size;

    return VEC_ALLOC_OK;
}

int vector_autoinit(vector_t *vector, size_t m, int percent)
{
    size_t elems;

    srand(time(NULL));

    elems = (size_t)((double)(m) * (double)percent / 100.0) ? (size_t)((double)(m) * (double)percent / 100.0) : 1;

    if (vector_alloc_data(vector, m))
        return VEC_INIT_ERR_ALLOC;

    while (elems)
        for (size_t j = 0; elems && j < m; j++)
            if (!vector->coords[j] && (rand() % (100 + 1)) <= percent)
            {
                while ((vector->coords[j] = rand() % (MAX_NUM - MIN_NUM) + MIN_NUM) == 0);
                
                elems--;
            }

    vector->full_size = m;

    return VEC_INIT_OK;
}

int vector_autoinit_by_user(vector_t *vector, size_t m, int percent)
{
    size_t elems;

    srand(time(NULL));

    elems = (size_t)((double)(m) * (double)percent / 100.0) ? (size_t)((double)(m) * (double)percent / 100.0) : 1;

    if (vector_alloc_data(vector, m))
        return VEC_INIT_ERR_ALLOC;

    while (elems)
        for (size_t j = 0; elems && j < m; j++)
            if (!vector->coords[j] && (rand() % (100 + 1)) <= percent)
            {
                printf("Enter %zu element (%zu elements remaining): ", j, elems);
                if (scanf("%d", &vector->coords[j]) != 1 || vector->coords[j] == 0)
                        return VEC_INIT_ERR_INVALID_ENTERED_DATA;

                elems--;
            }

    vector->full_size = m;

    return VEC_INIT_OK;
}

int vector_str_to_vector(vector_t *dst, vector_str_t *src)
{
    vector_free(dst);

    if (vector_alloc_data(dst, src->full_size))
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

void vector_output(vector_t *vector)
{
    size_t str_size = 0;
    size_t i = 0;
    printf("full_size = %zu\n", vector->full_size);

    while (str_size < vector->full_size)
    {
        printf(" index |   ");
        i = str_size;
        for (; (i % STR_TABLE_SIZE != 0 || i == str_size) && i < vector->full_size;)
            printf("%-*zu", CELL_SIZE, i++);
        printf("\n");

        printf("coords |   ");
        i = str_size;
        for (; (i % STR_TABLE_SIZE != 0 || i == str_size) && i < vector->full_size;)
            printf("%-*d", CELL_SIZE, vector->coords[i++]);

        str_size += i - str_size == STR_TABLE_SIZE ? STR_TABLE_SIZE : i % STR_TABLE_SIZE;

        printf("\n\n");
    }
}

int vector_init_manual(vector_t *vector, size_t m)
{
    bool is_zero_vector = true;

    vector_free(vector);

    if (vector_alloc_data(vector, m))
        return VEC_INIT_ERR_ALLOC;

    for (size_t j = 0; j < m; j++)
        if (scanf("%d", &vector->coords[j]) != 1)
            return VEC_INIT_ERR_FILL;
        else if (vector->coords[j] != 0)
            is_zero_vector = false;

    if (is_zero_vector)
        return VEC_INIT_ERR_ZERO_VECTOR;

    vector->full_size = m;

    return VEC_INIT_OK;
}

int vector_init_coords(vector_t *vector, size_t m)
{
    bool is_zero_vector = true;
    int tmp;
    size_t size_fill = 0;
    size_t ch_j;

    vector_free(vector);

    if (vector_alloc_data(vector, m))
        return VEC_INIT_ERR_ALLOC;

    while (size_fill <= m)
    {
        printf("Enter random int value (if you want to stop, enter -1): ");
        if (scanf("%d", &tmp) != 1)
        {
            vector_free(vector);
            return VEC_INIT_ERR_FILL;
        }

        if (tmp == -1)
            break;

        printf("Enter column (entered %zu of %zu): ", size_fill, m);
        if (scanf("%zu", &ch_j) != 1 || ch_j >= m)
            return VEC_INIT_ERR_FILL;

        printf("Enter element (non zero): ");
        if (scanf("%d", &tmp) != 1 || tmp == 0)
            return VEC_INIT_ERR_FILL;
        else if (tmp != 0)
            is_zero_vector = false;

        vector->coords[ch_j] = tmp;
        size_fill++;
    }

    if (is_zero_vector)
        return VEC_INIT_ERR_ZERO_VECTOR;

    vector->full_size = m;

    return VEC_INIT_OK;
}

size_t vector_mem(vector_t *vector)
{
    return sizeof(*vector) + sizeof(*(vector->coords)) * vector->full_size;
}
