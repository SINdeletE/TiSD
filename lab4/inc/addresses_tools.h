#ifndef ADDRESS_TOOLS_H__

#define ADDRESS_TOOLS_H__

#include <stddef.h>

#define CAP_CONST 2

typedef struct
{
    void **addresses;

    size_t size;
    size_t cap;
} addresses_t;

void addresses_free(void);
size_t addresses_cap(void);
size_t addresses_size(void);

int addresses_init(void);
int addresses_realloc(void);
int addresses_add(void *a);
int addresses_is_empty(void);
void addresses_show(void);

#endif