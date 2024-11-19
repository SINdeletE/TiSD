#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "file_tools.h"

int file_is_correct(char *filename)
{
    FILE *f = NULL;
    char *word = NULL;
    size_t size = 0;

    int has_data = 0;
    char *tmp = NULL;

    if (tmp = strchr(filename, '\n'))
    {
        *tmp = '\0';
        tmp = NULL;
    }

    f = fopen(filename, "r");
    if (! f)
        return PRCS_ERR_INVALID_FILENAME;

    while (getline(&word, &size, f) != -1)
        has_data = 1;

    if (! feof(f) && ferror(f))
    {
        fclose(f);

        return PRCS_ERR_INVALID_FILE_DATA;
    }

    fclose(f);
    
    return PRCS_OK;
}
