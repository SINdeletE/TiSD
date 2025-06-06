#include <stdio.h>
#include <string.h>
#include <math.h>

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

int erat(size_t num)
{
    for (size_t i = 2; i < (size_t)sqrt(num) + 1; i++)
        if (num % i == 0)
            return 0;
    
    return 1;
}

// ----------

size_t open_prime_nearest(size_t num)
{
    int flag = 1;

    // size_t left = num;
    size_t right = num;

    if (num < 2)
        return OPEN_SIZE_INIT; // MIN HASH SIZE
    
    if (num > TABLE_MAX_SIZE)
        return TABLE_MAX_SIZE;
    
    while (flag)
    {
        if (right < TABLE_MAX_SIZE)
        {
            right++;

            if (erat(right))
                return right;
        }
        else if (right == TABLE_MAX_SIZE)
            return TABLE_MAX_SIZE;
    }

    return OPEN_SIZE_INIT;
}

size_t open_hash_new_size(open_hash_table_t *hash_table)
{
    size_t elems_count = hash_table->elems_count;
    size_t new_size;
    size_t res;

    new_size = OPEN_NEW_SIZE(elems_count);
    res = open_prime_nearest(new_size);
    while (res <= hash_table->size && res != TABLE_MAX_SIZE)
    {
        elems_count++;

        new_size = OPEN_NEW_SIZE(elems_count);
        res = open_prime_nearest(new_size);
    }

    return res;
}

// ----------

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
    if (! tmp)
        return tmp;
        
    tmp->size = OPEN_SIZE_INIT;
    tmp->hash_function = binary_poly_hash_function;
    tmp->elems_count = 0;
    tmp->comp_limit = TABLE_INIT_COMP_LIMIT;

    return tmp;
}

double open_hash_compares(open_hash_table_t *hash_table)
{
    int total = 0;

    int current_comps;

    for (size_t i = 0; i < hash_table->size; i++)
    {
        current_comps = 1;

        if (hash_table->data[i])
            for (data_t *cur = hash_table->data[i]; cur; cur = cur->next)
                total += (current_comps++);
    }

    return (double)total / hash_table->elems_count;
}

// --------------------------------------------------

data_t *data_alloc(void)
{
    data_t *tmp = NULL;

    tmp = calloc(1, sizeof(struct data));

    return tmp;
}

int data_add(data_t **data, char *str, int *comp)
{
    data_t *tmp = NULL;
    data_t **cur = NULL;

    (*comp)++;
    for (cur = data; *cur; cur = &((*cur)->next))
    {
        (*comp)++;

        if (! strcmp((*cur)->str, str))
            return HASH_PRCS_ERR_SAME_DATA;
    }
    
    tmp = data_alloc();
    if (! tmp)
        return HASH_PRCS_ERR_ALLOC;
    tmp->str = str;

    *cur = tmp;


    return HASH_PRCS_OK;
}

int open_hash_table_add(open_hash_table_t *hash_table, char *str, int *comp)
{
    int res;
    size_t hash = 0;

    hash = hash_table->hash_function(str, hash_table->size);
    res = data_add(&hash_table->data[hash], str, comp);
    if (! res)
        hash_table->elems_count++;

    return res;
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

    hash = hash_table->hash_function(str, hash_table->size);
    
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

int open_hash_table_search(open_hash_table_t *hash_table, char *str, int *comp)
{
    size_t hash = 0;

    hash = hash_table->hash_function(str, hash_table->size);
    
    return data_search(hash_table->data[hash], str, comp);
}

// --------------------------------------------------

void data_output(data_t *data, size_t hash)
{
    printf("Hash: %zu | ", hash);

    for (data_t *cur = data; cur; cur = cur->next)
    {
        printf("\"");
        fputs(cur->str, stdout);
        printf("\"");

        printf(" ");
    }

    printf("\n");
}

void open_hash_table_output(open_hash_table_t *hash_table)
{
    printf("HASH TABLE:\n\n");

    for (size_t i = 0; i < hash_table->size; i++)
        data_output(hash_table->data[i], i);
}

// --------------------------------------------------

int open_hash_table_restruct(open_hash_table_t **hash_table, size_t new_size, size_t (*new_hash_function)(char *, size_t))
{
    int compares = 0;

    open_hash_table_t *new_hash_table = NULL;

    new_hash_table = open_hash_table_init();
    if (! new_hash_table)
        return HASH_PRCS_ERR_ALLOC;

    new_hash_table->size = new_size;
    new_hash_table->hash_function = new_hash_function;
    new_hash_table->comp_limit = (*hash_table)->comp_limit;

    for (size_t i = 0; i < (*hash_table)->size; i++)
        for (data_t *cur = (*hash_table)->data[i]; cur; cur = cur->next)
        {
            if (open_hash_table_add(new_hash_table, cur->str, &compares))
            {
                open_hash_table_free(&new_hash_table);
                open_hash_table_free(hash_table);

                return HASH_PRCS_ERR_ALLOC;
            }

            cur->str = NULL;
        }
    
    open_hash_table_free(hash_table);
    *hash_table = new_hash_table;

    return PRCS_OK;
}

// --------------------------------------------------

size_t open_hash_table_size(open_hash_table_t *hash_table)
{
    size_t total_size = sizeof(hash_table);

    total_size += sizeof(open_hash_table_t);

    for (size_t i = 0; i < TABLE_MAX_SIZE; i++)
        if (hash_table->data[i])
            for (data_t *cur = hash_table->data[i]; cur; cur = cur->next)
                total_size += sizeof(data_t) + (strlen(hash_table->data[i]->str) + 1) * sizeof(char);
    
    return total_size;
}

int open_hash_table_read_by_file(char *filedata, open_hash_table_t **hash_table)
{
    FILE *f = NULL;
    size_t filesize = 0;
    int flag = 0;

    char *word = NULL;
    size_t size = 0;

    int compares = 0;
    
    char *word_tmp = NULL;

    if (file_is_correct(filedata, &filesize))
        return READ_ERR_INVALID_FILE;

    f = fopen(filedata, "r");

    while (getline(&word, &size, f) != -1)
    {
        if ((word_tmp = strchr(word, '\n')))
        {
            *word_tmp = '\0';
            word_tmp = NULL;
        }

        switch (open_hash_table_add((*hash_table), word, &compares))
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
    
    while (open_hash_compares((*hash_table)) - (*hash_table)->comp_limit > -EPS && (*hash_table)->size < TABLE_MAX_SIZE)
        if (open_hash_table_restruct(hash_table, open_hash_new_size(*hash_table), (*hash_table)->hash_function))
        {
            open_hash_table_free(hash_table);

            return READ_ERR_INVALID_ALLOC;
        }

    return READ_OK;
}




























// ----------

size_t close_prime_nearest(size_t num)
{
    int flag = 1;

    size_t right = num;

    if (num < 2)
        return CLOSE_SIZE_INIT; // MIN HASH SIZE
    
    if (num >= TABLE_MAX_SIZE)
        return TABLE_MAX_SIZE;
    
    while (flag)
    {
        if (right < TABLE_MAX_SIZE)
        {
            right++;

            if (erat(right))
                return right;
        }
        else if (right == TABLE_MAX_SIZE)
            return TABLE_MAX_SIZE;
    }

    return CLOSE_SIZE_INIT;
}

size_t close_hash_new_size(close_hash_table_t *hash_table)
{
    size_t elems_count = hash_table->elems_count;
    size_t new_size;
    size_t res;

    new_size = CLOSE_NEW_SIZE(elems_count);
    res = close_prime_nearest(new_size);
    while (res <= hash_table->size && res != TABLE_MAX_SIZE)
    {
        elems_count++;

        new_size = CLOSE_NEW_SIZE(elems_count);
        res = close_prime_nearest(new_size);
    }

    return res;
}

// ----------

void close_hash_table_free(close_hash_table_t **hash_table)
{
    if (! hash_table || ! *hash_table)
        return;

    for (size_t i = 0; i < TABLE_MAX_SIZE; i++)
        if ((*hash_table)->data[i] != PILL)
            free((*hash_table)->data[i]);

    free(*hash_table);
    *hash_table = NULL;
}

void close_hashstat_data_clear(close_hash_table_t *hash_table)
{
    for (size_t i = 0; i < TABLE_MAX_SIZE; i++)
    {
        if (hash_table->data[i] != PILL)
            free(hash_table->data[i]);

        hash_table->data[i] = NULL;
    }
}

close_hash_table_t *close_hash_table_init(void)
{
    close_hash_table_t *tmp = NULL;

    tmp = calloc(1, sizeof(close_hash_table_t));
    if (! tmp)
        return tmp;

    tmp->size = CLOSE_SIZE_INIT;
    tmp->hash_function = binary_poly_hash_function;
    tmp->elems_count = 0;
    tmp->comp_limit = TABLE_INIT_COMP_LIMIT;

    return tmp;
}

double close_hash_compares(close_hash_table_t *hash_table)
{
    int total = 0;

    int current_comps = 0;

    for (size_t i = 0; i < hash_table->size; i++)
    {
        current_comps = 0;

        if (hash_table->data[i] && hash_table->data[i] != PILL)
        {
            close_hash_table_search(hash_table, hash_table->data[i], &current_comps);

            total += current_comps;
        }
    }

    return (double)total / hash_table->elems_count;
}

// --------------------------------------------------

int close_hash_table_data_add(close_hash_table_t *hash_table, size_t hash, char *str, int *comp)
{
    size_t i = 0;
    int flag = 0;

    (*comp)++;
    i = hash;
    while (hash_table->data[i] && hash_table->data[i] != PILL)
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

        (*comp)++;
    }

    hash_table->data[i] = str;
    hash_table->elems_count++;

    return HASH_PRCS_OK;
}

int close_hash_table_add(close_hash_table_t *hash_table, char *str, int *comp)
{
    size_t hash = 0;

    hash = hash_table->hash_function(str, hash_table->size);

    return close_hash_table_data_add(hash_table, hash, str, comp);
}

// --------------------------------------------------

int close_hash_table_delete(close_hash_table_t *hash_table, char *str)
{
    size_t hash = 0;
    size_t i = 0;
    int flag = 0;

    size_t tmp = 0;

    hash = hash_table->hash_function(str, hash_table->size);

    if (! hash_table->data[hash])
        return HASH_PRCS_ERR_NO_DATA;

    i = hash;
    while (hash_table->data[i] && (hash_table->data[i] == PILL || strcmp(hash_table->data[i], str)))
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
    hash_table->data[i] = PILL;

    return HASH_PRCS_OK;
}

// --------------------------------------------------

int close_hash_table_search(close_hash_table_t *hash_table, char *str, int *comp)
{
    size_t hash = 0;
    size_t i = 0;
    int flag = 0;

    hash = hash_table->hash_function(str, hash_table->size);

    if (! hash_table->data[hash])
        return HASH_PRCS_ERR_NO_DATA;

    (*comp)++;
    i = hash;
    while (hash_table->data[i] && (hash_table->data[i] == PILL || strcmp(hash_table->data[i], str)))
    {
        if (i == hash_table->size - 1)
        {
            i = 0;
            flag = 1;
        }
        else
            i++;

        (*comp)++;
        if (! hash_table->data[i] || (flag && i == hash))
            return HASH_PRCS_ERR_NO_DATA;
    }

    return HASH_PRCS_OK;
}

// --------------------------------------------------

void close_hash_table_output(close_hash_table_t *hash_table)
{
    printf("HASH TABLE:\n\n");

    for (size_t i = 0; i < hash_table->size; i++)
    {
        printf("Hash: %zu | ", i);
        if (hash_table->data[i] != PILL && hash_table->data[i])
        {
            printf("\"");
            fputs(hash_table->data[i], stdout);
            printf("\"");

            if (hash_table->data[i] != PILL && hash_table->hash_function(hash_table->data[i], hash_table->size) != i)
                printf(" (hash = %zu)", hash_table->hash_function(hash_table->data[i], hash_table->size));
        }

        printf("\n");
    }
}

// --------------------------------------------------

int close_hash_table_restruct(close_hash_table_t **hash_table, size_t new_size, size_t (*new_hash_function)(char *, size_t))
{
    int compares = 0;

    close_hash_table_t *new_hash_table = NULL;

    new_hash_table = close_hash_table_init();
    if (! new_hash_table)
        return HASH_PRCS_ERR_ALLOC;

    new_hash_table->size = new_size;
    new_hash_table->hash_function = new_hash_function;

    for (size_t i = 0; i < (*hash_table)->size; i++)
        if ((*hash_table)->data[i])
        {
            close_hash_table_add(new_hash_table, (*hash_table)->data[i], &compares);

            (*hash_table)->data[i] = NULL;
        }

    close_hash_table_free(hash_table);
    *hash_table = new_hash_table;

    return PRCS_OK;
}

// --------------------------------------------------

size_t close_hash_table_size(close_hash_table_t *hash_table)
{
    size_t total_size = sizeof(hash_table);

    total_size += sizeof(close_hash_table_t);

    for (size_t i = 0; i < TABLE_MAX_SIZE; i++)
        if (hash_table->data[i])
            total_size += (strlen(hash_table->data[i]) + 1) * sizeof(char);
    
    return total_size;
}

// --------------------------------------------------

int close_hash_table_read_by_file(char *filedata, close_hash_table_t **hash_table)
{
    FILE *f = NULL;
    size_t filesize = 0;
    int flag = 0;

    char *word = NULL;
    size_t size = 0;

    int compares = 0;
    
    char *word_tmp = NULL;

    if (file_is_correct(filedata, &filesize))
        return READ_ERR_INVALID_FILE;

    f = fopen(filedata, "r");

    (*hash_table)->size = filesize + 1;

    while (getline(&word, &size, f) != -1)
    {
        if ((word_tmp = strchr(word, '\n')))
        {
            *word_tmp = '\0';
            word_tmp = NULL;
        }

        switch (close_hash_table_add(*hash_table, word, &compares))
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

    while (close_hash_compares((*hash_table)) - (*hash_table)->comp_limit > -EPS && (*hash_table)->size < TABLE_MAX_SIZE)
        if (close_hash_table_restruct(hash_table, close_hash_new_size(*hash_table), (*hash_table)->hash_function))
        {
            close_hash_table_free(hash_table);

            return READ_ERR_INVALID_ALLOC;
        }

    return READ_OK;
}
