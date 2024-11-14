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
    // ------------------------------------------------------------------------------

    static_queue_t first_static_queue = (static_queue_t) {{0}, 0, NULL, NULL};
    static_queue_t second_static_queue = (static_queue_t) {{0}, 0, NULL, NULL};

    static_init(&first_static_queue);
    static_init(&second_static_queue);

    // ------------------------------------------------------------------------------

    list_queue_t first_list_queue = (list_queue_t) {NULL, NULL};
    list_queue_t second_list_queue = (list_queue_t) {NULL, NULL};

    // ------------------------------------------------------------------------------

    double T1_MIN = 1.0, T1_MAX = 5.0;
    double T2_MIN = 0.0, T2_MAX = 3.0;
    double T3_MIN = 0.0, T3_MAX = 4.0;
    double T4_MIN = 0.0, T4_MAX = 1.0;

    double tmp;

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
        printf("\nT1: %.2lf - %.2lf\n", T1_MIN, T1_MAX);
        printf("T2: %.2lf - %.2lf\n", T2_MIN, T2_MAX);
        printf("T3: %.2lf - %.2lf\n", T3_MIN, T3_MAX);
        printf("T4: %.2lf - %.2lf\n", T4_MIN, T4_MAX);
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
            case CODE_INIT:
                printf("Enter T1_MIN (enter invalid value if you want to skip): ");
                tmp = T1_MIN;
                if (scanf("%lf", &T1_MIN) != 1){}
                if (clear_buf(stdin) || T1_MIN < -EPS)
                    T1_MIN = tmp;

                printf("Enter T1_MAX (enter invalid value if you want to skip): ");
                tmp = T1_MAX;
                if (scanf("%lf", &T1_MAX) != 1){}
                if (clear_buf(stdin) || T1_MAX - T1_MIN < -EPS)
                    T1_MAX = tmp;

                printf("Enter T2_MIN (enter invalid value if you want to skip): ");
                tmp = T2_MIN;
                if (scanf("%lf", &T2_MIN) != 1){}
                if (clear_buf(stdin) || T2_MIN < -EPS)
                    T2_MIN = tmp;

                printf("Enter T2_MAX (enter invalid value if you want to skip): ");
                tmp = T2_MAX;
                if (scanf("%lf", &T2_MAX) != 1){}
                if (clear_buf(stdin) || T2_MAX - T2_MIN < -EPS)
                    T2_MAX = tmp;

                printf("Enter T3_MIN (enter invalid value if you want to skip): ");
                tmp = T3_MIN;
                if (scanf("%lf", &T3_MIN) != 1){}
                if (clear_buf(stdin) || T3_MIN < -EPS)
                    T3_MIN = tmp;

                printf("Enter T3_MAX (enter invalid value if you want to skip): ");
                tmp = T3_MAX;
                if (scanf("%lf", &T3_MAX) != 1){}
                if (clear_buf(stdin) || T3_MAX - T3_MIN < -EPS)
                    T3_MAX = tmp;

                printf("Enter T4_MIN (enter invalid value if you want to skip): ");
                tmp = T4_MIN;
                if (scanf("%lf", &T4_MIN) != 1){}
                if (clear_buf(stdin) || T4_MIN < -EPS)
                    T4_MIN = tmp;

                printf("Enter T4_MAX (enter invalid value if you want to skip): ");
                tmp = T4_MAX;
                if (scanf("%lf", &T4_MAX) != 1){}
                if (clear_buf(stdin) || T4_MAX - T4_MIN < -EPS)
                    T4_MAX = tmp;

                break;
            case CODE_STATIC_SERVICE:
                static_service(&first_static_queue, &second_static_queue, T1_MIN, T1_MAX, T2_MIN, T2_MAX, T3_MIN, T3_MAX, T4_MIN, T4_MAX);
                static_free(&first_static_queue);
                static_free(&second_static_queue);

                break;
            case CODE_LIST_SERVICE:
                list_service(&first_list_queue, &second_list_queue, T1_MIN, T1_MAX, T2_MIN, T2_MAX, T3_MIN, T3_MAX, T4_MIN, T4_MAX);
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
            case CODE_COMPARE_TIMES:
                time_comparing(&first_static_queue, &second_static_queue, &first_list_queue, &second_list_queue, T1_MIN, T1_MAX, T2_MIN, T2_MAX, T3_MIN, T3_MAX, T4_MIN, T4_MAX);

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
