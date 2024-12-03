#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table_tools.h"
#include "file_tools.h"

// HASH FUNC

size_t binary_poly_hash_function(char *str, size_t size)
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

size_t ternary_poly_hash_function(char *str, size_t size)
{
    size_t p = 1;
    size_t hash = 0;

    for (size_t i = 0; str[i]; i++)
    {
        hash += str[i] * p;

        p *= 3;
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

    if (! hash_table || ! *hash_table)
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

int open_hash_table_add(open_hash_table_t *hash_table, size_t (*open_hash_function)(char *, size_t ), char *str)
{
    size_t hash = 0;

    hash = open_hash_function(str, hash_table->size);

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

int open_hash_table_delete(open_hash_table_t *hash_table, size_t (*open_hash_function)(char *, size_t ), char *str)
{
    size_t hash = 0;

    hash = open_hash_function(str, hash_table->size);
    
    return data_delete(&hash_table->data[hash], str);
}

// --------------------------------------------------

int data_search(data_t *data, char *str, int *comp)
{
    for (data_t *cur = data; cur; cur = cur->next)
    {
        (*comp)++;

        if (! strcmp (cur->str, str))
            return HASH_PRCS_OK;
    }

    return HASH_PRCS_ERR_NO_DATA;
}

int open_hash_table_search(open_hash_table_t *hash_table, size_t (*open_hash_function)(char *, size_t ), char *str, int *comp)
{
    size_t hash = 0;

    hash = open_hash_function(str, hash_table->size);
    
    return data_search(hash_table->data[hash], str, comp);
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

// void open_hash_table_delete_by_char(open_hash_table_t *hash_table, size_t (*open_hash_function)(char *, size_t ), char c)
// {
//     data_t *cur = NULL;
//     data_t *tmp = NULL;

//     for (size_t i = 0; i < hash_table->size; i++)
//     {
//         cur = hash_table->data[i];

//         while (cur)
//             if (*cur->str == c)
//             {
//                 tmp = cur;
//                 cur = cur->next;

//                 open_hash_table_delete(hash_table, open_hash_function, tmp->str);
//             }
//             else
//                 cur = cur->next;
//     }
// }

// --------------------------------------------------

int open_hash_table_read_by_file(char *filedata, open_hash_table_t *hash_table, size_t (*open_hash_function)(char *, size_t ))
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

        switch (open_hash_table_add(hash_table, open_hash_function, word))
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










void close_hash_table_free(close_hash_table_t **hash_table)
{
    for (size_t i = 0; i < (*hash_table)->size; i++)
        free((*hash_table)->data[i]);

    free(*hash_table);
    *hash_table = NULL;
}

close_hash_table_t *close_hash_table_init(void)
{
    close_hash_table_t *tmp = NULL;

    tmp = calloc(1, sizeof(close_hash_table_t));

    return tmp;
}

int close_hash_table_add(close_hash_table_t *hash_table, size_t (*close_hash_function)(char *, size_t ), char *str)
{
    size_t hash = 0;
    size_t i = 0;
    int flag = 0;

    hash = close_hash_function(str, hash_table->size);

    i = hash;
    while (hash_table->data[i])
    {
        if (! strcmp(hash_table->data[i], str))
            return HASH_PRCS_ERR_SAME_DATA;

        if (i == hash_table->size - 1)
        {
            i = 0;
            flag = 1;
        }
        else
            i++;

        if (flag && i == hash)
            return HASH_PRCS_ERR_MAX_SIZE;
    }

    hash_table->data[i] = str;

    return HASH_PRCS_OK;
}

int close_hash_table_delete(close_hash_table_t *hash_table, size_t (*close_hash_function)(char *, size_t ), char *str)
{
    size_t hash = 0;
    size_t i = 0;
    int flag = 0;

    size_t tmp = 0;

    hash = close_hash_function(str, hash_table->size);

    if (! hash_table->data[i])
        return HASH_PRCS_ERR_NO_DATA;

    i = hash;
    while (hash_table->data[i] && ! strcmp(hash_table->data[i], str))
    {
        if (i == hash_table->size - 1)
        {
            i = 0;
            flag = 1;
        }
        else
            i++;

        if (! hash_table->data[i] || (flag && i == hash))
            return HASH_PRCS_ERR_NO_DATA;
    }

    str_free(&hash_table->data[i], &tmp);

    return HASH_PRCS_OK;
}

int close_hash_table_search(close_hash_table_t *hash_table, size_t (*close_hash_function)(char *, size_t ), char *str)
{
    size_t hash = 0;
    size_t i = 0;
    int flag = 0;

    size_t tmp = 0;

    hash = close_hash_function(str, hash_table->size);

    if (! hash_table->data[i])
        return HASH_PRCS_ERR_NO_DATA;

    i = hash;
    while (hash_table->data[i] && ! strcmp(hash_table->data[i], str))
    {
        if (i == hash_table->size - 1)
        {
            i = 0;
            flag = 1;
        }
        else
            i++;

        if (! hash_table->data[i] || (flag && i == hash))
            return HASH_PRCS_ERR_NO_DATA;
    }

    return HASH_PRCS_OK;
}

void close_hash_table_output(close_hash_table_t *hash_table)
{
    printf("HASH TABLE:\n\n");

    for (size_t i = 0; i < hash_table->size; i++)
        if (hash_table->data[i])
        {
            printf("Hash: %zu | ", i);
            fputs(hash_table->data[i], stdout);
            printf("\n");
        }
}

int close_hash_table_read_by_file(char *filedata, close_hash_table_t *hash_table, size_t (*close_hash_function)(char *, size_t ))
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

        switch (close_hash_table_add(hash_table, close_hash_function, word))
        {
        case HASH_PRCS_ERR_SAME_DATA:
            str_free(&word, &size);

            break;
        case HASH_PRCS_ERR_MAX_SIZE:
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
