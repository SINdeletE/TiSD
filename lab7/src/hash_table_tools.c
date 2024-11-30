#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table_tools.h"
#include "file_tools.h"

// HASH FUNC

size_t hash_function(char *str, size_t size)
{
    size_t p = 1;
    size_t hash = 0;

    for (size_t i = 0; str[i]; i++)
    {
        hash += str[i] * p;

        p <<= 1;
    }

    hash %= size;

    return hash;
}

// HASH FUNC

void data_free(data_t *data)
{
    if (! data)
        return;

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
    
    free(*hash_table);
    *hash_table = NULL;
}

open_hash_table_t *open_hash_table_init(void)
{
    open_hash_table_t *tmp = NULL;

    tmp = calloc(1, sizeof(struct open_hash_table));
    tmp->size = TABLE_INIT_SIZE;

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
        if (! strcmp((*cur)->str, str))
            return HASH_PRCS_ERR_SAME_DATA;
    
    tmp = data_alloc();
    if (! tmp)
        return HASH_PRCS_ERR_ALLOC;
    tmp->str = str;

    *cur = tmp;

    return HASH_PRCS_OK;
}

int open_hash_table_add(open_hash_table_t *hash_table, char *str)
{
    size_t hash = 0;

    hash = hash_function(str, hash_table->size);

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
        return HASH_PRCS_ERR_NO_DATA;

    return HASH_PRCS_OK;
}

int open_hash_table_delete(open_hash_table_t *hash_table, char *str)
{
    size_t hash = 0;

    hash = hash_function(str, hash_table->size);
    
    return data_delete(&hash_table->data[hash], str);
}

// --------------------------------------------------

int data_search(data_t *data, char *str)
{
    for (data_t *cur = data; cur; cur = cur->next)
        if (! strcmp (cur->str, str))
            return HASH_PRCS_OK;

    return HASH_PRCS_ERR_NO_DATA;
}

int open_hash_table_search(open_hash_table_t *hash_table, char *str)
{
    size_t hash = 0;

    hash = hash_function(str, hash_table->size);
    
    return data_search(hash_table->data[hash], str);
}

// --------------------------------------------------

void data_output(data_t *data, size_t hash)
{
    printf("Hash: %zu | ", hash);

    for (data_t *cur = data; cur; cur = cur->next)
    {
        fputs(cur->str, stdout);

        printf(" ");
    }

    printf("\n");
}

void open_hash_table_output(open_hash_table_t *hash_table)
{
    printf("HASH TABLE:\n\n");

    for (size_t i = 0; i < hash_table->size; i++)
        if (hash_table->data[i])
            data_output(hash_table->data[i], i);
}

// --------------------------------------------------

int open_hash_table_read_by_file(char *filedata, open_hash_table_t *hash_table)
{
    FILE *f = NULL;
    int flag = 0;

    char *word = NULL;
    size_t size = 0;
    
    char *word_tmp = NULL;

    if (file_is_correct(filedata))
        return READ_ERR_INVALID_FILE;

    f = fopen(filedata, "r");

    while (getline(&word, &size, f) != -1)
    {
        if ((word_tmp = strchr(word, '\n')))
        {
            *word_tmp = '\0';
            word_tmp = NULL;
        }

        switch (open_hash_table_add(hash_table, word))
        {
        case HASH_PRCS_ERR_ALLOC:
            str_free(&word, &size);

            return READ_ERR_INVALID_ALLOC;
        case HASH_PRCS_ERR_SAME_DATA:
            str_free(&word, &size);

            break;
        case HASH_PRCS_OK:
            flag = 1;
        } 

        str_unpin(&word, &size);
    }

    str_free(&word, &size);
    fclose(f);

    if (! flag)
        return READ_ERR_NO_DATA;

    return READ_OK;
}
