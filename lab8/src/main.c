#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "graph_tools.h"

#define _POSIX_C_SOURCE 200809L

#define EPS 1e-8

#define IO_OK 0
#define IO_ERR_MEM 1

#define CODE_READ 1
#define CODE_PATH 2
#define CODE_OUTPUT 3
#define CODE_EXIT 8

int clear_buf(FILE *f);

int main(void)
{
    graph_t *graph = NULL;

    // READ
    char *p = NULL;
    char *filename = NULL;
    size_t filename_size = 0;
    // READ

    // FIND PATH
    char *A = NULL;
    char *B = NULL;

    size_t tmp_size = 0;
    // FIND PATH

    bool flag = true;
    int code;
    graph_error_t func_code = 0;

    while (flag)
    {   
        printf("\n--------------------------------\n");
        printf("\nEnter a number of command:\n");
        printf("1. Read graph (from file)\n");
        printf("2. Find shortest path from A to B (instant result output)\n");
        printf("3. Output graph\n");
        printf("\n8. Exit\n");
        printf("\n--------------------------------\n");

        printf("Code: ");
        if (scanf("%d", &code) != 1)
            code = 0;
            
        if (clear_buf(stdin))
            code = 0;

        switch (code)
        {
            case CODE_READ:
                printf("Enter filename: ");
                if (getline(&filename, &filename_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((p = strchr(filename, '\n')))
                    *p = '\0';

                graph_free(&graph);
                
                switch (graph_read_from_file(&graph, filename))
                {
                case GRAPH_ERR_ALLOC:
                    printf("\nINVALID ALLOC\n");
                    
                    break;
                case GRAPH_ERR_NO_DATA:
                    printf("\nNO DATA IN FILE\n");
                    
                    break;
                case GRAPH_ERR_INVALID_FILE:
                    printf("\nINVALID FILENAME OR ISN'T EXIST\n");
                    
                    break;
                case GRAPH_ERR_INVALID_DATA:
                    printf("\nINVALID DATA\n");
                    
                    break;
                case GRAPH_ERR_SAME_DATA:
                    printf("\nSAME DATA\n");
                    
                    break;
                default:
                    printf("\nGRAPH WAS READ SUCCESSFULLY\n");

                    break;
                }

                str_free(&filename, &filename_size);

                break;
            case CODE_PATH:
                if (! graph)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("Enter A: ");
                if (getline(&A, &tmp_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((p = strchr(A, '\n')))
                    *p = '\0';

                tmp_size = 0;

                printf("Enter B: ");
                if (getline(&B, &tmp_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    str_free(&A, &tmp_size);
                    break;
                }

                if ((p = strchr(B, '\n')))
                    *p = '\0';

                tmp_size = 0;

                func_code = graph_way_find_path(graph, A, B);
                if (func_code == GRAPH_ERR_UNKNOWN_CITY)
                {
                    printf("\nINVALID CITY NAME: UNKNOWN CITY\n");

                    break;
                }

                str_free(&A, &tmp_size);
                str_free(&B, &tmp_size);

                break;
            case CODE_OUTPUT:
                if (! graph)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                graph_clear(graph);
                graph_output(graph, NULL);

                break;
            case CODE_EXIT:
                graph_free(&graph);
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
