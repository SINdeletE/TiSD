#ifndef STRING_TOOLS_H__

#define STRING_TOOLS_H__

#include <stdio.h>

#define STR_SIZE 256

#define STRING_OK 0
#define STRING_ERR_INVALID_READ 1
#define STRING_ERR_OVERFLOW 2
#define STRING_ERR_EMPTY_STROKE 3

int string_read(char *str, size_t size, FILE *f);

int clear_buf(FILE *f);
void stdin_clear_buf(void);

#endif
