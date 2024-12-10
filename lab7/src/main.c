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
#include "statistics.h"

#define _POSIX_C_SOURCE 200809L

#define EPS 1e-8

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

#define CODE_OPEN_RESTRUCT_SIZE 17
#define CODE_OPEN_HASH_READ 18
#define CODE_OPEN_HASH_ADD 19
#define CODE_OPEN_HASH_REMOVE 20
#define CODE_OPEN_HASH_SEARCH 21
#define CODE_OPEN_HASH_OUTPUT 22

#define CODE_CLOSE_RESTRUCT_SIZE 23
#define CODE_CLOSE_HASH_READ 24
#define CODE_CLOSE_HASH_ADD 25
#define CODE_CLOSE_HASH_REMOVE 26
#define CODE_CLOSE_HASH_SEARCH 27
#define CODE_CLOSE_HASH_OUTPUT 28

#define CODE_STAT 29

#define CODE_EXIT 30

int clear_buf(FILE *f);

int main(void)
{
    node_t *tree = NULL;
    node_t *avl_tree = NULL;

    double restruct_limit_tmp;

    open_hash_table_t *open_hash_table = NULL;
    close_hash_table_t *close_hash_table = NULL;

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
    char beg = 0;
    // // FIRST_IS_CHAR

    bool flag = true;
    int code;
    // int func_code = 0;

    while (flag)
    {   
        printf("\n--------------------------------\n");
        printf("\nEnter a number of command:\n");
        printf("\nUSUAL TREE\n");
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

        printf("\nOPEN HASHING HASH TABLE\n");
        if (open_hash_table)
        {
            printf("size: %zu\n", open_hash_table->size);
            if (open_hash_table->hash_function == binary_poly_hash_function)
                printf("hash function: binary\n");
            else if (open_hash_table->hash_function == ternary_poly_hash_function)
                printf("hash function: ternary\n");

            printf("restruct limit: %lf\n", open_hash_table->comp_limit);
        }
        printf("17. Enter restruct limit\n");
        printf("18. Read hash table (from file)\n");
        printf("19. Add element to hash table\n");
        printf("20. Delete element to hash table\n");
        printf("21. Search element to hash table\n");
        printf("22. Output hash table\n");

        printf("\nCLOSE HASHING HASH TABLE\n");
        if (close_hash_table)
        {
            printf("size: %zu\n", close_hash_table->size);
            if (close_hash_table->hash_function == binary_poly_hash_function)
                printf("hash function: binary\n");
            else if (close_hash_table->hash_function == ternary_poly_hash_function)
                printf("hash function: ternary\n");

            printf("restruct limit: %lf\n", close_hash_table->comp_limit);
        }
        printf("23. Enter restruct limit\n");
        printf("24. Read hash table (from file)\n");
        printf("25. Add element to hash table\n");
        printf("26. Delete element to hash table\n");
        printf("27. Search element to hash table\n");
        printf("28. Output hash table\n");

        printf("\n29. Get statistics\n");

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
            case CODE_FIRST_IS_CHAR:
                printf("Enter char: ");
                if (scanf("%c", &beg) != 1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                node_delete_by_char(&tree, beg);

                printf("\nNodes were deleted successfully\n");

                break;

// AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL AVL

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
            case CODE_AVL_FIRST_IS_CHAR:
                printf("Enter char: ");
                if (scanf("%c", &beg) != 1)
                {
                    printf("\nINVALID INPUT\n");

                    break;
                }

                avl_node_delete_by_char(&avl_tree, beg);

                printf("\nNodes were deleted successfully\n");

                break;

// OPEN HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH

            case CODE_OPEN_RESTRUCT_SIZE:
                if (! open_hash_table)
                {
                    printf("\nNO HASH TABLE\n");

                    break;
                }

                printf("Enter restruct value (> 1) (or enter invalid value to skip changes): ");
                if (scanf("%lf", &restruct_limit_tmp) == 1 && restruct_limit_tmp - 1 > EPS)
                {
                    open_hash_table->comp_limit = restruct_limit_tmp;

                    if (open_hash_compares(open_hash_table) - open_hash_table->comp_limit > -EPS && open_hash_table->size < TABLE_MAX_SIZE)
                    {
                        printf("Restructing hash table\n");
                        if (open_hash_table_restruct(&open_hash_table, open_hash_new_size(open_hash_table), open_hash_table->hash_function))
                            printf("\nINVALID RESTRUCT. HASH TABLE WAS DELETED\n");
                        else
                            printf("\nHASH TABLE WAS RESTRUCTED SUCCESSFULLY\n");
                    }
                }

                open_hash_table->comp_limit = restruct_limit_tmp;

                break;
            case CODE_OPEN_HASH_READ:
                open_hash_table_free(&open_hash_table);

                if (! (open_hash_table = open_hash_table_init()))
                {
                    printf("\nCOMPUTER CAN'T CREATE OPEN ADDRESS HASH TABLE\n");

                    break;
                }

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

                switch(open_hash_table_read_by_file(filename, &open_hash_table))
                {
                    case READ_ERR_NO_DATA:
                        printf("\nNO DATA IN FILE\n");

                        break;
                    case READ_ERR_INVALID_ALLOC:
                        printf("\nINVALID ALLOC\n");
                        open_hash_table = NULL;

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
                if (! open_hash_table && ! (open_hash_table = open_hash_table_init()))
                {
                    printf("\nCOMPUTER CAN'T CREATE OPEN ADDRESS HASH TABLE\n");

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
                switch (open_hash_table_add(open_hash_table, data, &compares))
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
                    printf("Total compares: %d\n", compares);
                    printf("Average compares: %.6lf\n", open_hash_compares(open_hash_table));

                    if (open_hash_compares(open_hash_table) - open_hash_table->comp_limit > -EPS && open_hash_table->size < TABLE_MAX_SIZE)
                    {
                        printf("Restructing hash table\n");
                        if (open_hash_table_restruct(&open_hash_table, open_hash_new_size(open_hash_table), open_hash_table->hash_function))
                            printf("\nINVALID RESTRUCT. HASH TABLE WAS DELETED\n");
                        else
                            printf("\nHASH TABLE WAS RESTRUCTED SUCCESSFULLY\n");
                    }
                }

                str_unpin(&data, &data_size);

                break;
            case CODE_OPEN_HASH_REMOVE:
                if (! open_hash_table)
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

                switch (open_hash_table_delete(open_hash_table, data))
                {
                case HASH_PRCS_ERR_NO_DATA:
                    printf("\nDATA IS NOT FOUND\n");

                    break;
                default:
                    printf("\nDATA WAS DELETED SUCCESSFULLY\n");

                    break;
                }

                str_free(&data, &data_size);

                break;
            case CODE_OPEN_HASH_SEARCH:
                if (! open_hash_table)
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
                if (! open_hash_table_search(open_hash_table, data, &compares))
                {
                    printf("\nDATA WAS FOUNDED SUCCESSFULLY\n");
                    printf("Total compares: %d\n", compares);
                    printf("Average compares: %.6lf\n", open_hash_compares(open_hash_table));

                    if (open_hash_compares(open_hash_table) - open_hash_table->comp_limit > -EPS && open_hash_table->size < TABLE_MAX_SIZE)
                    {
                        printf("Restructing hash table\n");
                        if (open_hash_table_restruct(&open_hash_table, open_hash_new_size(open_hash_table), open_hash_table->hash_function))
                            printf("\nINVALID RESTRUCT. HASH TABLE WAS DELETED\n");
                        else
                            printf("\nHASH TABLE WAS RESTRUCTED SUCCESSFULLY\n");
                    }
                }
                else
                {
                    printf("\nELEMENT IS NOT FOUND\n");
                    printf("Total compares: %d\n", compares);
                }

                str_free(&data, &data_size);

                break;
            case CODE_OPEN_HASH_OUTPUT:
                if (! open_hash_table)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                open_hash_table_output(open_hash_table);
                printf("\nAverage compares: %.6lf\n", open_hash_compares(open_hash_table));

                break;

// CLOSE HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH HASH
            
            case CODE_CLOSE_RESTRUCT_SIZE:
                if (! close_hash_table)
                {
                    printf("\nNO HASH TABLE\n");

                    break;
                }

                printf("Enter restruct value (> 1) (or enter invalid value to skip changes): ");
                if (scanf("%lf", &restruct_limit_tmp) == 1 && restruct_limit_tmp - 1 > EPS)
                {
                    close_hash_table->comp_limit = restruct_limit_tmp;

                    if (close_hash_compares(close_hash_table) - close_hash_table->comp_limit > -EPS && close_hash_table->size < TABLE_MAX_SIZE)
                    {
                        printf("Restructing hash table\n");
                        if (close_hash_table_restruct(&close_hash_table, close_hash_new_size(close_hash_table), close_hash_table->hash_function))
                            printf("\nINVALID RESTRUCT. HASH TABLE WAS DELETED\n");
                        else
                            printf("\nHASH TABLE WAS RESTRUCTED SUCCESSFULLY\n");
                    }
                }

                close_hash_table->comp_limit = restruct_limit_tmp;

                break;
            case CODE_CLOSE_HASH_READ:
                close_hash_table_free(&close_hash_table);

                if (! (close_hash_table = close_hash_table_init()))
                {
                    printf("\nCOMPUTER CAN'T CREATE CLOSE ADDRESS HASH TABLE\n");

                    break;
                }

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

                switch(close_hash_table_read_by_file(filename, &close_hash_table))
                {
                    case READ_ERR_NO_DATA:
                        printf("\nNO DATA IN FILE\n");

                        break;
                    case READ_ERR_INVALID_ALLOC:
                        printf("\nINVALID ALLOC\n");
                        close_hash_table = NULL;

                        break;
                    case READ_ERR_INVALID_FILE:
                        printf("\nINVALID FILE\n");

                        break;
                    default:
                        printf("\nCLOSE HASH TABLE WAS READ SUCCESSFULLY\n");
                }

                str_free(&filename, &filename_size);

                break;
            case CODE_CLOSE_HASH_ADD:
                if (! close_hash_table && ! (close_hash_table = close_hash_table_init()))
                {
                    printf("\nCOMPUTER CAN'T CREATE CLOSE ADDRESS HASH TABLE\n");

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
                if (close_hash_table->elems_count == close_hash_table->size && close_hash_table->size < TABLE_MAX_SIZE)
                {
                    printf("Restructing hash table (for size increasing)\n");
                    if (close_hash_table_restruct(&close_hash_table, close_hash_new_size(close_hash_table), close_hash_table->hash_function))
                    {
                        str_free(&data, &data_size);
                        close_hash_table_free(&close_hash_table);

                        printf("\nINVALID RESTRUCT. HASH TABLE WAS DELETED\n");
                    }
                    else
                        printf("\nHASH TABLE WAS RESTRUCTED SUCCESSFULLY\n");
                }

                compares = 0;
                switch (close_hash_table_add(close_hash_table, data, &compares))
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
                    printf("Total compares: %d\n", compares);
                    printf("Average compares: %.6lf\n", close_hash_compares(close_hash_table));

                    if (close_hash_compares(close_hash_table) - close_hash_table->comp_limit > -EPS && close_hash_table->size < TABLE_MAX_SIZE)
                    {
                        printf("Restructing hash table\n");
                        if (close_hash_table_restruct(&close_hash_table, close_hash_new_size(close_hash_table), close_hash_table->hash_function))
                            printf("\nINVALID RESTRUCT. HASH TABLE WAS DELETED\n");
                        else
                            printf("\nHASH TABLE WAS RESTRUCTED SUCCESSFULLY\n");
                    }
                }

                str_unpin(&data, &data_size);

                break;
            case CODE_CLOSE_HASH_REMOVE:
                if (! close_hash_table)
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

                switch (close_hash_table_delete(close_hash_table, data))
                {
                case HASH_PRCS_ERR_NO_DATA:
                    printf("\nDATA IS NOT FOUND\n");

                    break;
                default:
                    printf("\nDATA WAS DELETED SUCCESSFULLY\n");

                    break;
                }

                str_free(&data, &data_size);

                break;
            case CODE_CLOSE_HASH_SEARCH:
                if (! close_hash_table)
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
                if (! close_hash_table_search(close_hash_table, data, &compares))
                {
                    printf("\nDATA WAS FOUNDED SUCCESSFULLY\n");
                    printf("Total compares: %d\n", compares);
                    printf("Average compares: %.6lf\n", close_hash_compares(close_hash_table));

                    if (close_hash_compares(close_hash_table) - close_hash_table->comp_limit > -EPS && close_hash_table->size < TABLE_MAX_SIZE)
                    {
                        printf("Restructing hash table\n");
                        if (close_hash_table_restruct(&close_hash_table, close_hash_new_size(close_hash_table), close_hash_table->hash_function))
                            printf("\nINVALID RESTRUCT. HASH TABLE WAS DELETED\n");
                        else
                            printf("\nHASH TABLE WAS RESTRUCTED SUCCESSFULLY\n");
                    }
                }
                else
                {
                    printf("\nELEMENT IS NOT FOUND\n");
                    printf("Total compares: %d\n", compares);
                }

                str_free(&data, &data_size);

                break;
            case CODE_CLOSE_HASH_OUTPUT:
                if (! close_hash_table)
                {
                    printf("\nNO DATA\n");

                    break;
                }

                close_hash_table_output(close_hash_table);
                printf("\nAverage compares: %.6lf\n", close_hash_compares(close_hash_table));

                break;

// STATISTICS STATISTICS STATISTICS STATISTICS STATISTICS STATISTICS STATISTICS STATISTICS STATISTICS STATISTICS STATISTICS STATISTICS

            case CODE_STAT:    
                hashstat();

                total_stat_search();
                total_stat_compares();

                break;

// EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT EXIT

            case CODE_EXIT:
                tree = node_free(tree);
                avl_tree = node_free(avl_tree);
                open_hash_table_free(&open_hash_table);
                close_hash_table_free(&close_hash_table);

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
