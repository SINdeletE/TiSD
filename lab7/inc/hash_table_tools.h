#ifndef HASH_TABLE_TOOLS_H__

#define HASH_TABLE_TOOLS_H__

#include <stddef.h>

#define PRCS_OK 0
#define PRCS_ERR_ALLOC 1
#define PRCS_ERR_NO_DATA 2
#define PRCS_ERR_SAME_DATA 3

#define TABLE_MAX_SIZE 40000

// ---

typedef struct
{
    char *data[TABLE_MAX_SIZE];
} close_hash_table_t;

// ---

typedef struct data data_t;

struct data
{
    char *str;

    data_t *next;
};

typedef struct open_hash_table open_hash_table_t;

struct open_hash_table
{
    data_t *data[TABLE_MAX_SIZE];
};

// ---



void open_hash_table_free(open_hash_table_t **hash_table);
open_hash_table_t *open_hash_table_init(void);
int open_hash_table_add(open_hash_table_t *hash_table, char *str);
int open_hash_table_delete(open_hash_table_t *hash_table, char *str);
int open_hash_table_search(open_hash_table_t *hash_table, char *str);

#endif
