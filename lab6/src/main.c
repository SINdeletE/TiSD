#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "file_tools.h"
#include "node_tools.h"

#define _POSIX_C_SOURCE 200809L

#define IO_OK 0
#define IO_ERR_INVALID_DATA 1

#define CODE_READ 1
#define CODE_ADD 2
#define CODE_REMOVE 3
#define CODE_SEARCH 4
#define CODE_PRE_ORDER 5
#define CODE_IN_ORDER 6
#define CODE_POST_ORDER 7
#define CODE_EXIT 9

int clear_buf(FILE *f);

int main(void)
{
    node_t *tree = NULL;
    char *filename = NULL;
    size_t filename_size = 0;

    char *tmp = NULL;

    // VISUAL
    FILE *f = NULL;
    // VISUAL

    bool flag = true;
    int code;
    // int func_code = 0;

    while (flag)
    {   
        printf("\n--------------------------------\n");
        printf("\nEnter a number of command:\n");
        printf("1. Read tree (from file)\n");
        printf("2. Add tree element by data\n");
        printf("3. Remove tree element by data\n");
        printf("4. Search tree element by data\n");
        printf("5. Output tree (with pre-order) + Graphviz visualization\n");
        printf("6. Output tree (with in-order) + Graphviz visualization\n");
        printf("7. Output tree (with post-order) + Graphviz visualization\n");

        printf("\n9. Exit program\n");
        printf("\n--------------------------------\n");

        printf("Code: ");
        if (scanf("%d", &code) != 1)
            code = 0;
            
        if (clear_buf(stdin))
            code = 0;

        switch (code)
        {
            case CODE_READ:
                tree = node_free(tree);

                printf("Enter filename (with extension): ");
                if (getline(&filename, &filename_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((tmp = strchr(filename, '\n')))
                {
                    *tmp = '\0';
                    tmp = NULL;
                }

                switch(node_read_by_file(filename, &tree))
                {
                    case READ_ERR_NO_DATA:
                        printf("\nNO DATA IN FILE\n");

                        break;
                    case READ_ERR_INVALID_ALLOC:
                        printf("\nINVALID ALLOC\n");

                        break;
                    case READ_ERR_INVALID_FILE:
                        printf("\nINVALID FILE\n");

                        break;
                    default:
                        printf("\nTREE WAS READED SUCCESSFULLY\n");
                }

                str_free(&filename, &filename_size);

                break;
            case CODE_PRE_ORDER:
                if (! tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("PRE-ORDER:\n");
                node_output_pre_order(tree, stdout);

                f = fopen("TreeVisual.gv", "w");
                node_export_to_dot_eli(f, "TREE", tree);
                fclose(f);

                system("dot -Tpng TreeVisual.gv -o graph.png");

                break;
            case CODE_IN_ORDER:
                if (! tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("IN-ORDER:\n");
                node_output_in_order(tree, stdout);

                f = fopen("TreeVisual.gv", "w");
                node_export_to_dot_eli(f, "TREE", tree);
                fclose(f);

                system("dot -Tpng TreeVisual.gv -o graph.png");

                break;
            case CODE_POST_ORDER:
                if (! tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("POST-ORDER:\n");
                node_output_post_order(tree, stdout);

                f = fopen("TreeVisual.gv", "w");
                node_export_to_dot_eli(f, "TREE", tree);
                fclose(f);

                system("dot -Tpng TreeVisual.gv -o graph.png");

                break;
            case CODE_EXIT:
                tree = node_free(tree);
                str_free(&filename, &filename_size);

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
