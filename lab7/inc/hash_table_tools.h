#ifndef HASH_TABLE_TOOLS_H__

#define HASH_TABLE_TOOLS_H__

#include <stddef.h>

#define HASH_PRCS_OK 0
#define HASH_PRCS_ERR_ALLOC 1
#define HASH_PRCS_ERR_NO_DATA 2
#define HASH_PRCS_ERR_SAME_DATA 3
#define HASH_PRCS_ERR_MAX_SIZE 4

#define READ_OK 0
#define READ_ERR_NO_DATA 1
#define READ_ERR_INVALID_ALLOC 2
#define READ_ERR_INVALID_FILE 3

#define TABLE_MAX_SIZE 40000
#define TABLE_INIT_SIZE 3000

// ---

typedef struct
{
    char *data[TABLE_MAX_SIZE];
    size_t (*hash_function)(char *, size_t);

    size_t size;
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
    size_t (*hash_function)(char *, size_t);

    size_t size;
};

// ---

size_t binary_poly_hash_function(char *str, size_t size);
size_t ternary_poly_hash_function(char *str, size_t size);

void open_hash_table_free(open_hash_table_t **hash_table);
open_hash_table_t *open_hash_table_init(void);
int open_hash_table_read_by_file(char *filedata, open_hash_table_t *hash_table);
int open_hash_table_add(open_hash_table_t *hash_table, char *str, int *comp);
int open_hash_table_delete(open_hash_table_t *hash_table, char *str);
int open_hash_table_search(open_hash_table_t *hash_table, char *str, int *comp);
void open_hash_table_output(open_hash_table_t *hash_table);
// void open_hash_table_delete_by_char(open_hash_table_t *hash_table, char c);
int open_hash_table_restruct(open_hash_table_t **hash_table, size_t new_size, size_t (*new_hash_function)(char *, size_t));

// ------

void close_hash_table_free(close_hash_table_t **hash_table);
close_hash_table_t *close_hash_table_init(void);
int close_hash_table_read_by_file(char *filedata, close_hash_table_t *hash_table);
int close_hash_table_add(close_hash_table_t *hash_table, char *str, int *comp);
int close_hash_table_delete(close_hash_table_t *hash_table, char *str);
void close_hash_table_output(close_hash_table_t *hash_table);
int close_hash_table_search(close_hash_table_t *hash_table, char *str, int *comp);
int close_hash_table_restruct(close_hash_table_t **hash_table, size_t new_size, size_t (*new_hash_function)(char *, size_t));

#endif
