#include <stdlib.h>
#include <string.h>

#include "hash_table_tools.h"

// HASH FUNC

size_t hash_function(char *str)
{
    size_t p = 1;
    size_t hash = 0;

    for (size_t i = 0; str[i]; i++)
    {
        hash += str[i] * p;

        p <<= 1;
    }

    hash %= TABLE_MAX_SIZE;

    return hash;
}

// HASH FUNC

void data_free(data_t *data)
{
    free(data->str);
    free(data);
}

void open_hash_table_data_free(data_t **data)
{
    data_t *last = NULL;

    if (! *data)
        return;

    for (data_t *cur = *data; cur; cur = cur->next)
    {
        data_free(last);
        
        last = cur;
    }
    data_free(last);

    *data = NULL;
}

void open_hash_table_free(open_hash_table_t **hash_table)
{
    open_hash_table_t *table = *hash_table;

    if (! hash_table)
        return;

    for (size_t i = 0; i < TABLE_MAX_SIZE; i++)
        open_hash_table_data_free(&table->data[i]);
    
    *hash_table = NULL;
}

open_hash_table_t *open_hash_table_init(void)
{
    open_hash_table_t *tmp = NULL;

    tmp = calloc(1, sizeof(struct open_hash_table));

    return tmp;
}

// --------------------------------------------------

data_t *data_alloc(void)
{
    data_t *tmp = NULL;

    tmp = calloc(1, sizeof(struct data));

    return tmp;
}

int data_add(data_t **data, char *str)
{
    data_t *tmp = NULL;
    data_t **cur = NULL;

    for (cur = data; *cur; cur = &((*cur)->next))
        if (! strcmp ((*cur)->str, str))
            return PRCS_ERR_SAME_DATA;
    
    tmp = data_alloc();
    if (! tmp)
        return PRCS_ERR_ALLOC;

    *cur = tmp;

    return PRCS_OK;
}

int open_hash_table_add(open_hash_table_t *hash_table, char *str)
{
    size_t hash = 0;

    hash = hash_function(str);

    return data_add(&hash_table->data[hash], str);
}

// --------------------------------------------------

int data_delete(data_t **data, char *str)
{
    data_t *tmp = NULL;
    data_t **cur = NULL;
    int flag = 1;

    for (cur = data; flag && *cur; cur = &((*cur)->next))
        if (! strcmp ((*cur)->str, str))
        {
            tmp = *cur;
            *cur = (*cur)->next;

            data_free(tmp);

            flag = 0;
        }

    if (flag)
        return PRCS_ERR_NO_DATA;

    return PRCS_OK;
}

int open_hash_table_delete(open_hash_table_t *hash_table, char *str)
{
    size_t hash = 0;

    hash = hash_function(str);
    
    return data_delete(&hash_table->data[hash], str);
}

// --------------------------------------------------

int data_search(data_t *data, char *str)
{
    for (data_t *cur = data; cur; cur = cur->next)
        if (! strcmp (cur->str, str))
            return PRCS_OK;

    return PRCS_ERR_NO_DATA;
}

int open_hash_table_search(open_hash_table_t *hash_table, char *str)
{
    size_t hash = 0;

    hash = hash_function(str);
    
    return data_search(hash_table->data[hash], str);
}

// --------------------------------------------------
