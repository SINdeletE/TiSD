#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_tools.h"

void str_free(char **str, size_t *size)
{
    free(*str);
    *str = NULL;

    *size = 0;
}

void str_unpin(char **str, size_t *size)
{
    *str = NULL;

    *size = 0;
}

int file_is_correct(char *filename)
{
    FILE *f = NULL;
    char *word = NULL;
    size_t size = 0;

    int has_data = 0;

    f = fopen(filename, "r");
    if (! f)
        return PRCS_ERR_INVALID_FILENAME;

    while (getline(&word, &size, f) != -1)
    {
        has_data = 1;

        str_free(&word, &size);
    }

    str_free(&word, &size);

    if (! feof(f) && ferror(f))
    {
        fclose(f);

        return PRCS_ERR_INVALID_FILE_DATA;
    }

    fclose(f);

    if (! has_data)
        return PRCS_ERR_NO_DATA;
    
    return PRCS_OK;
}
