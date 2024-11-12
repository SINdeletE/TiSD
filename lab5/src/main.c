#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "static_queue_tools.h"
#include "list_queue_tools.h"
#include "service.h"
#include "addresses_tools.h"

#define _POSIX_C_SOURCE 200809L

#define IO_OK 0
#define IO_ERR_INVALID_DATA 1

#define CODE_STATIC_SERVICE 1
#define CODE_LIST_SERVICE 2
#define CODE_LIST_ADDRESSES 3
#define CODE_EXIT 4

int clear_buf(FILE *f);

int main(void)
{
    // ------------------------------------------------------------------------------

    static_queue_t first_static_queue = (static_queue_t) {{0}, 0, NULL, NULL};
    static_queue_t second_static_queue = (static_queue_t) {{0}, 0, NULL, NULL};

    static_init(&first_static_queue);
    static_init(&second_static_queue);

    // ------------------------------------------------------------------------------

    list_queue_t first_list_queue = (list_queue_t) {NULL, NULL};
    list_queue_t second_list_queue = (list_queue_t) {NULL, NULL};

    // ------------------------------------------------------------------------------

    bool flag = true;
    int code;
    // int func_code = 0;

    if (addresses_init())
    {
        printf("\nCAN'T ALLOC MEMORY FOR ADDRESSES\n");

        return 1;
    }

    while (flag)
    {   
        printf("\n--------------------------------\n");
        printf("\nEnter a number of command:\n");
        printf("1. Execute service (static queue)\n");
        printf("2. Execute service (list queue)\n");
        printf("3. Output addresses");

        printf("\n4. Exit program\n");
        printf("\n--------------------------------\n");

        printf("Code: ");
        if (scanf("%d", &code) != 1)
            code = 0;
            
        if (clear_buf(stdin))
            code = 0;

        switch (code)
        {
            case CODE_STATIC_SERVICE:
                static_service(&first_static_queue, &second_static_queue);
                static_free(&first_static_queue);
                static_free(&second_static_queue);

                break;
            case CODE_LIST_SERVICE:
                list_service(&first_list_queue, &second_list_queue);
                list_free(&first_list_queue);
                list_free(&second_list_queue);

                break;
            case CODE_LIST_ADDRESSES:
                printf("\nALL ADDRESSES\n");
                addresses_show();

                if (addresses_fragcheck())
                    printf("\nFRAGMENTATION: NO\n");
                else
                    printf("\nFRAGMENTATION: YES\n");

                break;
            case CODE_EXIT:
                list_free(&first_list_queue);
                list_free(&second_list_queue);
                addresses_free();

                flag = false;

                break;
            default:
                printf("\nINVALID CODE\n");
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
