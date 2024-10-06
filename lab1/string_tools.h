#ifndef STRING_TOOLS_H

#define STRING_TOOLS_H

#include <stddef.h>

#include "num_tools.h"

#define OK 0
#define EMPTY_STRING 1
#define OVERFLOW_BUFFER 2
#define ERROR_READ 3
#define ERROR 555

#define SIGNS "+-"
#define EXP "eE"
#define DOT '.'
#define DIGITS "0123456789"

#define STR_LEN 256

#define ZERO_CONST '0'

int input_str(char *str, int max_str_len);
int trim_spaces(char *dst, const char *src);

int convert_to_real(char *str, real_t *real);
int convert_to_inum(char *str, inum_t *inum);

#endif
