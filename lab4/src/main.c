#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "stack_tools.h"
#include "string_tools.h"

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
#define CODE_STAT 12
#define CODE_EXIT 13

int clear_buf(FILE *f);

int main(void)
{
    char *str = NULL;
    size_t alloc_str = 0;
    
    static_stack_t static_stack = (static_stack_t) {{0}, 0};
    list_stack_t *list_stack_head = NULL;

    void *tmp = NULL;;

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

        printf("\n12. Get statistics\n");

        printf("\n13. Exit program\n");
        printf("\n--------------------------------\n");

        printf("Code: ");
        if (scanf("%d", &code) != 1)
            code = 0;
            
        if (clear_buf(stdin))
            code = 0;

        switch (code)
        {
            case CODE_INIT:
                if (str == NULL)
                {
                    if ((str = malloc(sizeof(char))) == NULL)
                    {
                        printf("\nCOMPUTER CAN'T ALLOC MEMORY FOR STRING\n");

                        break;
                    }
                    else
                        alloc_str = sizeof(char);
                }
                
                printf("Enter string: ");
                if (getline(&str, &alloc_str, stdin) == -1)
                {
                    printf("\nCOMPUTER CAN'T ALLOC MEMORY FOR STRING\n");

                    string_free(&str, &alloc_str);
                    break;
                }

                if (strcmp(str, "\n") == 0)
                {
                    printf("\nSTRING CAN'T BE EMPTY\n");

                    string_free(&str, &alloc_str);
                    break;
                }

                printf("\nSTRING WAS ENTERED SUCCESSFULLY\n");

                break;
            case CODE_SHOW_STATIC:
                printf("\nStack (last to first):\n");
                static_stack_show(&static_stack);

                break;
            case CODE_POP_STATIC:
                if (static_stack_pop(&character, &static_stack))
                    printf("\nSTACK IS EMPTY\n");
                else
                {
                    printf("\nPOP SUCCESSFULLY\n");
                    printf("Popped: %c\n", character);
                }

                break;
            case CODE_PUSH_STATIC:
                printf("Enter character: ");
                if (scanf("%c", &character) != 1)
                    printf("\nINVALID ENTERED DATA\n");
                else
                {
                    if (static_stack_push(character, &static_stack))
                        printf("\nSTACK IS OVERFLOWED\n");
                    else
                        printf("\nPUSH SUCCESSFULLY\n");
                }

                break;
            case CODE_CLEAN_STATIC:
                static_stack_free(&static_stack);
                printf("\nSTATIC STACK WAS CLEARED SUCCESSFULLY\n");

                break;
            case CODE_SHOW_LIST:
                printf("\nStack (last to first):\n");
                list_stack_show(list_stack_head);

                break;
            case CODE_POP_LIST:
                tmp = (void *)list_stack_head;

                if (list_stack_pop(&character, &list_stack_head))
                {
                    printf("\nSTACK IS EMPTY\n");

                    break;
                }
                else
                {
                    printf("\nPOP SUCCESSFULLY\n");
                    printf("Popped: %c %p\n", character, tmp);
                }

                break;
            case CODE_PUSH_LIST:
                printf("Enter character: ");
                if (scanf("%c", &character) != 1)
                    printf("\nINVALID ENTERED DATA\n");
                else
                {
                    if (list_stack_push(character, &list_stack_head))
                        printf("\nCAN'T ALLOC MEMORY FOR NEW ELEMENT\n");
                    else
                        printf("\nPUSH SUCCESSFULLY\n");
                }

                break;
            case CODE_CLEAN_LIST:
                list_stack_free(&list_stack_head);
                printf("\nLIST STACK WAS CLEARED SUCCESSFULLY\n");

                break;
            case CODE_CHECK_STATIC:
                if (str == NULL)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("\nString: ");
                fputs(str, stdout);
                switch (static_stack_string_check(str, &static_stack))
                {
                    case CHECK_ERR_STACK_INCORRECT_PUSH:
                        printf("\nSTACK WAS OVERFLOWED. CAN'T GET RESULT\n");

                        break;
                    case CHECK_INCORRECT:
                        printf("\nResult: INCORRECT\n");

                        break;
                    case CHECK_CORRECT:
                        printf("\nResult: CORRECT\n");
                }

                break;
            case CODE_CHECK_LIST:
                if (str == NULL)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("\nString: ");
                fputs(str, stdout);
                switch (list_stack_string_check(str, &list_stack_head))
                {
                    case CHECK_ERR_STACK_INCORRECT_PUSH:
                        printf("\nCOULDN'T ALLOC MEMORY FOR PUSH A CHARACTER\n");

                        break;
                    case CHECK_INCORRECT:
                        printf("\nResult: INCORRECT\n");

                        break;
                    case CHECK_CORRECT:
                        printf("\nResult: CORRECT\n");
                }

                break;
            case CODE_STAT:
                
            case CODE_EXIT:
                string_free(&str, &alloc_str);
                list_stack_free(&list_stack_head);

                printf("\nHave a nice day!\n");
                flag = false;

                break;
            default:
                printf("\nINVALID ENTERED DATA\n");
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
