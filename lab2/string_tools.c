#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include "string_tools.h"

int string_read(char *str, size_t size, FILE *f)
{
    char *tmp = NULL;

    if (fgets(str, size, f) != str)
        return STRING_ERR_INVALID_READ;

    if ((tmp = strchr(str, '\n')) == NULL)
        return STRING_ERR_OVERFLOW;

    *tmp = '\0';

    if (strcmp(str, "") == 0)
        return STRING_ERR_EMPTY_STROKE;

    return STRING_OK;
}

int clear_buf(FILE *f)
{
    int ch;

    do
    {
        ch = getc(f);
        if (! isspace(ch) && ch != EOF)
            return 1;
    } while (ch != '\n' && ch != EOF);

    return 0;
}

void stdin_clear_buf(void)
{
    int ch;

    do
    {
        ch = getchar();
    } while (ch != '\n' && ch != EOF);
}
