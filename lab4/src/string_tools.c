#include <stdlib.h>

#include "string_tools.h"

void string_free(char **str, size_t *alloc_memory)
{
    free(*str);

    *str = NULL;
    *alloc_memory = 0;
}