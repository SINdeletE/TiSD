#ifndef FILE_TOOLS_H__

#define FILE_TOOLS_H__

#include <stddef.h>

#define PRCS_OK 0
#define PRCS_ERR_INVALID_FILENAME 1
#define PRCS_ERR_INVALID_FILE_DATA 2
#define PRCS_ERR_NO_DATA 3

#define EPS 1e-8

void str_free(char **str, size_t *size);
void str_unpin(char **str, size_t *size);

int file_is_correct(char *filename, size_t *size);
size_t file_search_by_char(char *filedata, char c);

#endif
