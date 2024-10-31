#include <stdio.h>
#include <stdlib.h>

#include "addresses_tools.h"

static addresses_t as;

void addresses_free(void)
{
    free(as.addresses);
    as.cap = 0;
    as.size = 0;
}

size_t addresses_cap(void)
{
    return as.cap;
}

size_t addresses_size(void)
{
    return as.size;
}

int addresses_init(void)
{
    as = (addresses_t) {NULL, 0, 0};

    if (! (as.addresses = malloc(sizeof(void *) * 1)))
        return 1;
    
    as.cap = 1;
    as.size = 0;
    
    return 0;
}

int addresses_realloc(void)
{
    void **tmp = NULL;

    if (! (tmp = realloc(as.addresses, sizeof(void *) * (as.cap * CAP_CONST))))
        return 1;

    as.addresses = tmp;
    as.cap *= CAP_CONST;
    
    return 0;
}

int addresses_add(void *a)
{
    if (as.size == as.cap)
        if (addresses_realloc())
            return 1;

    as.addresses[as.size] = a;
    as.size++;

    return 0;
}

int addresses_is_empty(void)
{
    if (as.addresses == NULL)
        return 1;

    return 0;
}

void addresses_show(void)
{
    for (size_t i = 0; i < as.size; i++)
        printf("%zu. %p\n", i + 1, as.addresses[i]);
}
