#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "node_tools.h"

#define _POSIX_C_SOURCE 200809L

#define EPS 1e-8

#define IO_OK 0
#define IO_ERR_INVALID_DATA 1

#define CODE_INIT 1
#define CODE_STATIC_SERVICE 2
#define CODE_LIST_SERVICE 3
#define CODE_LIST_ADDRESSES 4
#define CODE_COMPARE_TIMES 5
#define CODE_EXIT 6

int clear_buf(FILE *f);

int main(void)
{
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
        printf("1. Change T values\n");
        printf("2. Execute service (static queue)\n");
        printf("3. Execute service (list queue)\n");
        printf("4. Output addresses\n");
        printf("5. Compare REAL worktimes\n");

        printf("\n6. Exit program\n");
        printf("\n--------------------------------\n");

        printf("Code: ");
        if (scanf("%d", &code) != 1)
            code = 0;
            
        if (clear_buf(stdin))
            code = 0;

        switch (code)
        {
            

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
