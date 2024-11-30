#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "file_tools.h"
#include "node_tools.h"
#include "avl_tools.h"
#include "hash_table_tools.h"

#define _POSIX_C_SOURCE 200809L

#define IO_OK 0
#define IO_ERR_MEM 1

#define CODE_READ 1
#define CODE_ADD 2
#define CODE_REMOVE 3
#define CODE_SEARCH 4
#define CODE_PRE_ORDER 5
#define CODE_IN_ORDER 6
#define CODE_POST_ORDER 7
#define CODE_FIRST_IS_CHAR 8

#define CODE_AVL_CPY 9
#define CODE_AVL_ADD 10
#define CODE_AVL_REMOVE 11
#define CODE_AVL_SEARCH 12
#define CODE_AVL_PRE_ORDER 13
#define CODE_AVL_IN_ORDER 14
#define CODE_AVL_POST_ORDER 15
#define CODE_AVL_FIRST_IS_CHAR 16

#define CODE_OPEN_HASH_READ 17
#define CODE_OPEN_HASH_ADD 18
#define CODE_OPEN_HASH_REMOVE 19
#define CODE_OPEN_HASH_SEARCH 20
#define CODE_OPEN_HASH_OUTPUT 21
#define CODE_OPEN_HASH_FIRST_IS_CHAR 22

#define CODE_EXIT 30

int clear_buf(FILE *f);

int main(void)
{
    node_t *tree = NULL;
    node_t *avl_tree = NULL;
    open_hash_table_t *open_hash_table = NULL;
    size_t open_hash_table_size = TABLE_INIT_SIZE;
    

    char *filename = NULL;
    size_t filename_size = 0;

    char *tmp = NULL;

    char *data = NULL;
    size_t data_size = 0;

    // ADD
    node_t *add_tmp = NULL;
    // ADD

    // SEARCH
    int compares = 0;
    // SEARCH

    // VISUAL
    FILE *f = NULL;
    // VISUAL

    // // FIRST_IS_CHAR
    // char beg = 0;
    // size_t count;
    // // FIRST_IS_CHAR

    bool flag = true;
    int code;
    // int func_code = 0;

    if (! (open_hash_table = open_hash_table_init()))
    {
        printf("\nCOMPUTER CAN'T CREATE OPEN ADDRESS HASH TABLE\n");

        return IO_ERR_MEM;
    }

    while (flag)
    {   
        printf("\n--------------------------------\n");
        printf("\nEnter a number of command:\n");
        printf("\nUSUAL TREE OPERATIONS\n");
        printf("1. Read tree (from file)\n");
        printf("2. Add tree element by data\n");
        printf("3. Remove tree element by data\n");
        printf("4. Search tree element by data\n");
        printf("5. Output tree (with pre-order) + Graphviz visualization\n");
        printf("6. Output tree (with in-order) + Graphviz visualization\n");
        printf("7. Output tree (with post-order) + Graphviz visualization\n");
        printf("8. Find elements by char and delete it\n");

        printf("\nAVL TREE\n");
        printf("9. Copy USUAL TREE and balance it to AVL TREE\n");
        printf("10. Add tree element by data\n");
        printf("11. Remove tree element by data\n");
        printf("12. Search tree element by data\n");
        printf("13. Output tree (with pre-order) + Graphviz visualization\n");
        printf("14. Output tree (with in-order) + Graphviz visualization\n");
        printf("15. Output tree (with post-order) + Graphviz visualization\n");
        printf("16. Find elements by char and delete it\n");

        printf("\nOPEN ADDRESS HASH TABLE\n");
        printf("17. Read hash table (from file)\n");
        printf("18. Add element to hash table\n");
        printf("19. Delete element to hash table\n");
        printf("20. Search element to hash table\n");
        printf("21. Output hash table\n");
        printf("22. Find elements by char and delete it\n");

        // printf("\n9. Get statistics\n");

        printf("\n30. Exit program\n");
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
                        printf("\nTREE WAS READ SUCCESSFULLY\n");
                        printf("tree height: %d", node_height(tree));
                }

                str_free(&filename, &filename_size);

                break;
            case CODE_ADD:
                printf("Enter str value: ");
                if (getline(&data, &data_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((tmp = strchr(data, '\n')))
                {
                    *tmp = '\0';
                    tmp = NULL;
                }

                add_tmp = node_alloc(data);
                if (! add_tmp)
                {
                    printf("\nINVALID ALLOC\n");
                    str_free(&data, &data_size);

                    break;
                }

                add_tmp->data = data;
                
                if (node_search(tree, data, &compares))
                {
                    node_free(add_tmp);
                    str_unpin(&data, &data_size);

                    printf("\nTHIS VALUE IS ALREADY IN TREE\n");
                    break;
                }

                tree = node_add(tree, add_tmp);
                str_unpin(&data, &data_size);

                printf("\nDATA WAS ADDED SUCCESSFULLY\n");

                break;
            case CODE_REMOVE:
                if (! tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("Enter str value: ");
                if (getline(&data, &data_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((tmp = strchr(data, '\n')))
                {
                    *tmp = '\0';
                    tmp = NULL;
                }

                if (node_search(tree, data, &compares))
                {
                    node_delete(&tree, data);

                    printf("\nDATA WAS DELETED SUCCESSFULLY\n");
                }
                else
                    printf("\nELEMENT IS NOT FOUND\n");

                str_free(&data, &data_size);

                break;
            case CODE_SEARCH:
                if (! tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("Enter str value: ");
                if (getline(&data, &data_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((tmp = strchr(data, '\n')))
                {
                    *tmp = '\0';
                    tmp = NULL;
                }

                compares = 0;
                if (node_search(tree, data, &compares))
                    printf("\nDATA WAS FOUNDED SUCCESSFULLY\n");
                else
                    printf("\nELEMENT IS NOT FOUND\n");

                printf("Total compares: %d\n", compares);

                str_free(&data, &data_size);

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
            // case CODE_FIRST_IS_CHAR:
            //     printf("Enter char: ");
            //     if (scanf("%c", &beg) != 1)
            //     {
            //         printf("\nINVALID INPUT\n");

            //         break;
            //     }

            //     count = node_count_and_color(tree, beg);

            //     printf("\nTotal count: %zu\n", count);
            //     printf("Nodes were colored\n");

            //     break;
            case CODE_AVL_CPY:
                if (! tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                avl_tree = node_free(avl_tree);

                if (tree_to_avl_cpy(tree, &avl_tree))
                    printf("\nMEMORY ERROR\n");
                else
                    printf("\nTREE WAS COPIED TO AVL\n");

                break;
            case CODE_AVL_ADD:
                printf("Enter str value: ");
                if (getline(&data, &data_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((tmp = strchr(data, '\n')))
                {
                    *tmp = '\0';
                    tmp = NULL;
                }

                add_tmp = node_alloc(data);
                if (! add_tmp)
                {
                    printf("\nINVALID ALLOC\n");
                    str_free(&data, &data_size);

                    break;
                }

                add_tmp->data = data;
                
                if (node_search(avl_tree, data, &compares))
                {
                    node_free(add_tmp);
                    str_unpin(&data, &data_size);

                    printf("\nTHIS VALUE IS ALREADY IN TREE\n");
                    break;
                }

                avl_tree = avl_node_add(avl_tree, add_tmp);
                str_unpin(&data, &data_size);

                printf("\nDATA WAS ADDED SUCCESSFULLY\n");

                break;
            case CODE_AVL_REMOVE:
                if (! avl_tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("Enter str value: ");
                if (getline(&data, &data_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((tmp = strchr(data, '\n')))
                {
                    *tmp = '\0';
                    tmp = NULL;
                }

                if (node_search(avl_tree, data, &compares))
                {
                    avl_node_delete(&avl_tree, data);

                    printf("\nDATA WAS DELETED SUCCESSFULLY\n");
                }
                else
                    printf("\nELEMENT IS NOT FOUND\n");

                str_free(&data, &data_size);

                break;
            case CODE_AVL_SEARCH:
                if (! avl_tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("Enter str value: ");
                if (getline(&data, &data_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((tmp = strchr(data, '\n')))
                {
                    *tmp = '\0';
                    tmp = NULL;
                }

                compares = 0;
                if (node_search(avl_tree, data, &compares))
                    printf("\nDATA WAS FOUNDED SUCCESSFULLY\n");
                else
                    printf("\nELEMENT IS NOT FOUND\n");

                printf("Total compares: %d\n", compares);

                str_free(&data, &data_size);

                break;
            case CODE_AVL_PRE_ORDER:
                if (! avl_tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("PRE-ORDER:\n");
                node_output_pre_order(avl_tree, stdout);

                f = fopen("TreeVisual.gv", "w");
                node_export_to_dot_eli(f, "TREE", avl_tree);
                fclose(f);

                system("dot -Tpng TreeVisual.gv -o graph.png");

                break;
            case CODE_AVL_IN_ORDER:
                if (! avl_tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("IN-ORDER:\n");
                node_output_in_order(avl_tree, stdout);

                f = fopen("TreeVisual.gv", "w");
                node_export_to_dot_eli(f, "TREE", avl_tree);
                fclose(f);

                system("dot -Tpng TreeVisual.gv -o graph.png");

                break;
            case CODE_AVL_POST_ORDER:
                if (! avl_tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("POST-ORDER:\n");
                node_output_post_order(avl_tree, stdout);

                f = fopen("TreeVisual.gv", "w");
                node_export_to_dot_eli(f, "TREE", avl_tree);
                fclose(f);

                system("dot -Tpng TreeVisual.gv -o graph.png");

                break;
            case CODE_OPEN_HASH_READ:
                open_hash_table_size = open_hash_table->size;

                open_hash_table_free(&open_hash_table);

                if (! (open_hash_table = open_hash_table_init()))
                {
                    printf("\nCOMPUTER CAN'T CREATE OPEN ADDRESS HASH TABLE\n");

                    break;
                }
                open_hash_table->size = open_hash_table_size;

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

                switch(open_hash_table_read_by_file(filename, open_hash_table))
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
                        printf("\nOPEN HASH TABLE WAS READ SUCCESSFULLY\n");
                }

                str_free(&filename, &filename_size);

                break;
            case CODE_OPEN_HASH_ADD:
                printf("Enter str value: ");
                if (getline(&data, &data_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((tmp = strchr(data, '\n')))
                {
                    *tmp = '\0';
                    tmp = NULL;
                }

                switch (open_hash_table_add(open_hash_table, data))
                {
                case HASH_PRCS_ERR_ALLOC:
                    printf("\nINVALID ALLOCATE\n");
                    str_free(&data, &data_size);

                    break;
                case HASH_PRCS_ERR_SAME_DATA:
                    printf("\nDATA IS ALREADY EXIST\n");
                    str_free(&data, &data_size);

                    break;
                default:
                    printf("\nDATA WAS ADDED SUCCESSFULLY\n");
                }

                str_unpin(&data, &data_size);

                break;
            case CODE_OPEN_HASH_REMOVE:
                if (! tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("Enter str value: ");
                if (getline(&data, &data_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((tmp = strchr(data, '\n')))
                {
                    *tmp = '\0';
                    tmp = NULL;
                }

                if (node_search(tree, data, &compares))
                {
                    node_delete(&tree, data);

                    printf("\nDATA WAS DELETED SUCCESSFULLY\n");
                }
                else
                    printf("\nELEMENT IS NOT FOUND\n");

                str_free(&data, &data_size);

                break;
            case CODE_OPEN_HASH_SEARCH:
                if (! tree)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                printf("Enter str value: ");
                if (getline(&data, &data_size, stdin) == -1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                if ((tmp = strchr(data, '\n')))
                {
                    *tmp = '\0';
                    tmp = NULL;
                }

                compares = 0;
                if (node_search(tree, data, &compares))
                    printf("\nDATA WAS FOUNDED SUCCESSFULLY\n");
                else
                    printf("\nELEMENT IS NOT FOUND\n");

                printf("Total compares: %d\n", compares);

                str_free(&data, &data_size);

                break;
            case CODE_OPEN_HASH_OUTPUT:
                open_hash_table_output(open_hash_table);

                break;
            case CODE_EXIT:
                tree = node_free(tree);
                avl_tree = node_free(avl_tree);
                open_hash_table_free(&open_hash_table);

                str_free(&filename, &filename_size);
                str_free(&data, &data_size);

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
