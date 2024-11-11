#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "static_queue_tools.h"
#include "list_queue_tools.h"

#define _POSIX_C_SOURCE 200809L

#define IO_OK 0
#define IO_ERR_INVALID_DATA 1

#define CODE_INIT 1
#define CODE_SHOW_STATIC 2
#define CODE_POP_STATIC 3
#define CODE_PUSH_STATIC 4
#define CODE_CLEAN_STATIC 5
#define CODE_SHOW_LIST 6
#define CODE_POP_LIST 7
#define CODE_PUSH_LIST 8
#define CODE_CLEAN_LIST 9
#define CODE_CHECK_STATIC 10
#define CODE_CHECK_LIST 11
#define CODE_ADDRESSES 12
#define CODE_STAT 13
#define CODE_EXIT 14

int clear_buf(FILE *f);

int main(void)
{
    static_queue_t first_static_queue = (static_queue_t) {{0}, 0, NULL, NULL};
    static_queue_t second_static_queue = (static_queue_t) {{0}, 0, NULL, NULL};

    list_queue_t first_list_queue = (list_queue_t) {NULL, NULL};
    list_queue_t second_list_queue = (list_queue_t) {NULL, NULL};

    bool flag = true;
    int code;
    // int func_code = 0;

    char character;

    while (flag)
    {   
        printf("\n--------------------------------\n");
        printf("\nEnter a number of command:\n");
        printf("1. Initialize expression\n");
        printf("\n2. Show (static stack)\n");
        printf("3. Pop (static stack)\n");
        printf("4. Push (static stack)\n");
        printf("5. Clean (static stack)\n");
        printf("\n6. Show (list stack)\n");
        printf("7. Pop (list stack)\n");
        printf("8. Push (list stack)\n");
        printf("9. Clean (list stack)\n");
        printf("\n10. Check brackets correction (static stack)\n");
        printf("11. Check brackets correction (list stack)\n");

        printf("\n12. Show popped addresses\n");

        printf("\n13. Get statistics\n");

        printf("\n14. Exit program\n");
        printf("\n--------------------------------\n");

        printf("Code: ");
        if (scanf("%d", &code) != 1)
            code = 0;
            
        if (clear_buf(stdin))
            code = 0;

        switch (code)
        {
            default:

        }
    }

    return IO_OK;
}

int clear_buf(FILE *f)
{
    int ch;
    int flag = 0;

    do
    {
        ch = getc(f);
        if (! isspace(ch) && ch != EOF)
            flag = 1;
    } while (ch != '\n' && ch != EOF);

    return flag;
}
