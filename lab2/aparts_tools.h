#ifndef APARTS_TOOLS_H__

#define APARTS_TOOLS_H__

#include <stddef.h>

#include "apart_tools.h"

#define READ_OK 0
#define READ_ERR_INVALID_FILENAME 1
#define READ_ERR_NO_FILE 2
#define READ_ERR_INVALID_FILE 3
#define READ_ERR_DATA_ALLOC 4
#define READ_ERR_INVALID_APART 5
#define READ_ERR_NO_DATA 6

#define APART_READ_OK 0
#define APART_READ_ERR 1
#define APART_READ_EOF 2

// ---

#define APART_ADD_OK 0
#define APART_ADD_ERR_INVALID_DATA_BUF 1
#define APART_ADD_ERR_INVALID_DATA_NO_BUF 2
#define APART_ADD_ERR_DATA_ALLOC 3

// ---

#define APART_DELETE_OK 0
#define APART_DELETE_ERR_NO_DATA 1
#define APART_DELETE_ERR_DATA_ALLOC 2

// ---

#define APART_WRITE_OK 0
#define APART_WRITE_ERR_FILE_OPEN 1
#define APART_WRITE_ERR_INVALID_FILENAME 2
#define APART_WRITE_ERR_NO_DATA 3

// --- 

#define APART_SORT_OK 0
#define APART_SORT_ERR_ALLOC 1

// ---

#define APART_BY_PRICE_OK 0
#define APART_BY_PRICE_ERR_NO_SECONDARY 1
#define APART_BY_PRICE_ERR_NO_DATA 2

// ---

#define APART_STAT_OK 0
#define APART_STAT_ERR_ALLOC 1

int aparts_read(apart_t **aparts, size_t *size);
void aparts_output(apart_t *aparts, size_t size);
int aparts_add(apart_t **aparts, size_t *size, size_t i);
int aparts_delete(apart_t **aparts, size_t *size, size_t i);
int aparts_save(apart_t *aparts, size_t size);
int aparts_output_by_price(apart_t *aparts, size_t size, double min_price, double max_price);

int insertion_sort(void *base, size_t num, size_t size, int (*compare)(const void *, const void *, void *(*)(apart_t *)), void *(*key)(apart_t *));
int gnome_sort(void *base, size_t num, size_t size, int (*compare)(const void *, const void *, void *(*)(apart_t *)), void *(*key)(apart_t *));

int aparts_strcmp(const void *p1, const void *p2, void *(*param) (apart_t *));
int aparts_doublecmp(const void *p1, const void *p2, void *(*param) (apart_t *));
int aparts_intcmp(const void *p1, const void *p2, void *(*param) (apart_t *));

// KEY

int statistics_get(apart_t *aparts, size_t size, keystat_t *keys);

void aparts_output_by_keys(apart_t *aparts, size_t size, keystat_t *keys);
int keys_create(apart_t *aparts, size_t size, keystat_t **keys);
int key_insertion_sort(void *base, size_t num, size_t size, int (*compare)(const void *, const void *, void *(*)(keystat_t *)), void *(*key)(keystat_t *));
int key_gnome_sort(void *base, size_t num, size_t size, int (*compare)(const void *, const void *, void *(*)(keystat_t *)), void *(*key)(keystat_t *));

int keys_intcmp(const void *p1, const void *p2, void *(*param) (keystat_t *));

void output_keys(keystat_t *keys, size_t size);

// KEY

#endif
