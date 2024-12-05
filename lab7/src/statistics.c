#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "statistics.h"

#define MIN_INDEX 0
#define MAX_INDEX 5

#define ZERO_CHAR '0'
#define DEC_CONST 10

#define STR_TABLE_SIZE 8

void hashstat_data_str_create(char *name, size_t size)
{
    for (int i = MAX_INDEX - 1; i >= MIN_INDEX; i--)
    {
        name[i] = ZERO_CHAR + size % DEC_CONST;

        size /= DEC_CONST;
    }
}

void open_hashstat_data_add(open_hash_table_t *hash_table, size_t hash, size_t count, char *last_str)
{
    char *str = NULL;
    size_t size = 0;

    int comp = 0;

    for (size_t i = 0; i < count - 1; i++)
    {
        str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
        hashstat_data_str_create(str, i);
        data_add(&hash_table->data[hash], str, &comp);

        str_unpin(&str, &size);
    }

    str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
    strcpy(str, last_str);
    data_add(&hash_table->data[hash], str, &comp);

    str_unpin(&str, &size);
}

void close_hashstat_data_add(close_hash_table_t *hash_table, size_t hash, size_t count, char *last_str)
{
    char *str = NULL;
    size_t size = 0;

    int comp = 0;

    for (size_t i = 0; i < count - 1; i++)
    {
        str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
        hashstat_data_str_create(str, i);
        close_hash_table_data_add(hash_table, hash, str, &comp);

        str_unpin(&str, &size);
    }

    str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
    strcpy(str, last_str);
    close_hash_table_data_add(hash_table, hash, str, &comp);

    str_unpin(&str, &size);
}

void avl_data_add(node_t **node, size_t count)
{
    node_t *tmp = NULL;

    char *str = NULL;
    size_t size = 0;

    for (size_t i = 0; i < count; i++)
    {
        str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
        hashstat_data_str_create(str, i);
        tmp = node_alloc(str);

        *node = avl_node_add(*node, tmp);

        str_unpin(&str, &size);
    }
}

void hashstat(void)
{
    char hash_searching_data[HASHSTAT_STR_SIZE] = "Searchable";

    int compares = 0;

    open_hash_table_t *open_hash_table = NULL;
    close_hash_table_t *close_hash_table = NULL;

    // ---

    struct timespec t_beg, t_end;

    double open_time_search = 0;
    int open_time_search_compares = 0;

    double close_time_search = 0;
    int close_time_search_compares = 0;

    // ---

    open_hash_table = open_hash_table_init();
    if (! open_hash_table)
        return;
    
    close_hash_table = close_hash_table_init();
    if (! close_hash_table)
        return;

    // ---

    printf("\nHASHES STATISTICS (time in nsec) (total iteration: %d)\n", ITER_COUNT);

    printf("\nBINARY HASH FUNCTION; SIZE = %zu\n", open_hash_table->size);
    printf("collision|");
    printf("HASH FIND (OPEN)|");
    printf("HASH FIND(CLOSE)|");
    printf("COMPARES  (OPEN)|");
    printf("COMPARES (CLOSE)|");
    printf("   best time    |\n");
    for (size_t coll_count = MIN_COLL_COUNT; coll_count < MAX_COLL_COUNT; coll_count *= COLL_STEP)
    {
        hash_searching_data[0] = ZERO_CHAR + coll_count;

        // ---

        open_hashstat_data_add(open_hash_table, open_hash_table->hash_function(hash_searching_data, open_hash_table->size), coll_count, hash_searching_data);

        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            open_hash_table_search(open_hash_table, hash_searching_data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            open_time_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            open_time_search_compares = compares;

            compares = 0;
        }

        for (size_t i = 0; i < open_hash_table->size; i++)
            open_hash_table_data_free(&open_hash_table->data[i]);

        // ---

        close_hashstat_data_add(close_hash_table, close_hash_table->hash_function(hash_searching_data, close_hash_table->size), coll_count, hash_searching_data);

        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            close_hash_table_search(close_hash_table, hash_searching_data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            close_time_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            close_time_search_compares = compares;

            compares = 0;
        }

        close_hashstat_data_clear(close_hash_table);

        printf("%-*zu|", STR_TABLE_SIZE + 1, coll_count - 1);
        printf("%-*lf|", STR_TABLE_SIZE * 2, open_time_search / MAX_ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2, close_time_search / MAX_ITER_COUNT);
        printf("%-*d|", STR_TABLE_SIZE * 2, open_time_search_compares);
        printf("%-*d|", STR_TABLE_SIZE * 2, close_time_search_compares);
        if (open_time_search < close_time_search)
            printf("      OPEN      |");
        else if (open_time_search > close_time_search)
            printf("      CLOSE     |");
        else 
            printf("      EQUAL     |");

        printf("\n");

        open_time_search = 0.0;
        close_time_search = 0.0;

        open_time_search_compares = 0;
        close_time_search_compares = 0;
    }

    open_hash_table_free(&open_hash_table);
    close_hash_table_free(&close_hash_table);

    // ---

    printf("\nTERNARY HASH FUNCTION; SIZE = %zu\n", (size_t)TABLE_INIT_SIZE * 2);
    printf("collision|");
    printf("HASH FIND (OPEN)|");
    printf("HASH FIND(CLOSE)|");
    printf("COMPARES  (OPEN)|");
    printf("COMPARES (CLOSE)|");
    printf("   best time    |\n");
    for (size_t coll_count = MIN_COLL_COUNT; coll_count < MAX_COLL_COUNT; coll_count *= COLL_STEP)
    {
        hash_searching_data[0] = ZERO_CHAR + coll_count;

        // ---

        open_hash_table = open_hash_table_init();
        open_hashstat_data_add(open_hash_table, open_hash_table->hash_function(hash_searching_data, open_hash_table->size), coll_count, hash_searching_data);
        open_hash_table_restruct(&open_hash_table, TABLE_INIT_SIZE * 2, ternary_poly_hash_function);

        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            open_hash_table_search(open_hash_table, hash_searching_data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            open_time_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            open_time_search_compares = compares;

            compares = 0;
        }

        for (size_t i = 0; i < open_hash_table->size; i++)
            open_hash_table_data_free(&open_hash_table->data[i]);

        // ---

        close_hash_table = close_hash_table_init();
        close_hashstat_data_add(close_hash_table, close_hash_table->hash_function(hash_searching_data, close_hash_table->size), coll_count, hash_searching_data);
        close_hash_table_restruct(&close_hash_table, TABLE_INIT_SIZE * 2, ternary_poly_hash_function);

        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            close_hash_table_search(close_hash_table, hash_searching_data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            close_time_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            close_time_search_compares = compares;

            compares = 0;
        }

        close_hashstat_data_clear(close_hash_table);

        printf("%-*zu|", STR_TABLE_SIZE + 1, coll_count - 1);
        printf("%-*lf|", STR_TABLE_SIZE * 2, open_time_search / MAX_ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2, close_time_search / MAX_ITER_COUNT);
        printf("%-*d|", STR_TABLE_SIZE * 2, open_time_search_compares);
        printf("%-*d|", STR_TABLE_SIZE * 2, close_time_search_compares);
        if (open_time_search < close_time_search)
            printf("      OPEN      |");
        else if (open_time_search > close_time_search)
            printf("      CLOSE     |");
        else 
            printf("      EQUAL     |");

        printf("\n");

        open_time_search = 0.0;
        close_time_search = 0.0;

        open_time_search_compares = 0;
        close_time_search_compares = 0;

        open_hash_table_free(&open_hash_table);
        close_hash_table_free(&close_hash_table);

        // ---
    }

    // ---

    open_hash_table = open_hash_table_init();
    if (! open_hash_table)
        return;
    open_hash_table_restruct(&open_hash_table, TABLE_INIT_SIZE * 2, ternary_poly_hash_function);
    
    
    close_hash_table = close_hash_table_init();
    if (! close_hash_table)
        return;
    close_hash_table_restruct(&close_hash_table, TABLE_INIT_SIZE * 2, ternary_poly_hash_function);

    // ---

    printf("\nTERNARY HASH FUNCTION; SIZE = %zu\n", open_hash_table->size);
    printf("collision|");
    printf("HASH FIND (OPEN)|");
    printf("HASH FIND(CLOSE)|");
    printf("COMPARES  (OPEN)|");
    printf("COMPARES (CLOSE)|");
    printf("   best time    |\n");
    for (size_t coll_count = MIN_COLL_COUNT; coll_count < MAX_COLL_COUNT; coll_count *= COLL_STEP)
    {
        hash_searching_data[0] = ZERO_CHAR + coll_count;

        // ---

        open_hashstat_data_add(open_hash_table, open_hash_table->hash_function(hash_searching_data, open_hash_table->size), coll_count, hash_searching_data);

        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            open_hash_table_search(open_hash_table, hash_searching_data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            open_time_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            open_time_search_compares = compares;

            compares = 0;
        }

        for (size_t i = 0; i < open_hash_table->size; i++)
            open_hash_table_data_free(&open_hash_table->data[i]);

        // ---

        close_hashstat_data_add(close_hash_table, close_hash_table->hash_function(hash_searching_data, close_hash_table->size), coll_count, hash_searching_data);

        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            close_hash_table_search(close_hash_table, hash_searching_data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            close_time_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            close_time_search_compares = compares;

            compares = 0;
        }

        close_hashstat_data_clear(close_hash_table);

        printf("%-*zu|", STR_TABLE_SIZE + 1, coll_count - 1);
        printf("%-*lf|", STR_TABLE_SIZE * 2, open_time_search / MAX_ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2, close_time_search / MAX_ITER_COUNT);
        printf("%-*d|", STR_TABLE_SIZE * 2, open_time_search_compares);
        printf("%-*d|", STR_TABLE_SIZE * 2, close_time_search_compares);
        if (open_time_search < close_time_search)
            printf("      OPEN      |");
        else if (open_time_search > close_time_search)
            printf("      CLOSE     |");
        else 
            printf("      EQUAL     |");

        printf("\n");

        open_time_search = 0.0;
        close_time_search = 0.0;

        open_time_search_compares = 0;
        close_time_search_compares = 0;
    }

    open_hash_table_free(&open_hash_table);
    close_hash_table_free(&close_hash_table);

    // ---
}





































void avl_hash_stat(void)
{
    node_t *avl_tree = NULL;
    open_hash_table_t *open_hash_table = NULL;
    close_hash_table_t *close_hash_table = NULL;

    char hash_searching_data[HASHSTAT_STR_SIZE] = "Searchable";
    char avl_searching_data[HASHSTAT_STR_SIZE] = {0};

    node_t *tmp = NULL;

    char *str = NULL;
    size_t size = 0;

    int compares = 0;

    // ---

    struct timespec t_beg, t_end;

    double open_time_delete = 0;
    int open_time_delete_compares = 0;

    double close_time_delete = 0;
    int close_time_delete_compares = 0;

    double AVL_time_delete = 0;
    int AVL_time_delete_compares = 0;

    // ---

    // ---

    open_hash_table = open_hash_table_init();
    if (! open_hash_table)
        return;
    
    close_hash_table = close_hash_table_init();
    if (! close_hash_table)
        return;

    // ---

    printf("\nHASHES VS AVL STATISTICS (time in nsec) (total iteration: %d)\n", ITER_COUNT);
    printf("(P.S. count is (collision count + 1) for hash tables)\n");

    printf("\nBINARY HASH FUNCTION; SIZE = %zu\n", open_hash_table->size);
    printf(" count |");
    printf("T DELETE  (OP)|");
    printf("T DELETE  (CL)|");
    printf("T DELETE (AVL)|");
    printf("COMPARES  (OP)|");
    printf("COMPARES  (CL)|");
    printf("COMPARES (AVL)|");
    printf("   SIZE   (OP)|");
    printf("   SIZE   (CL)|");
    printf("   SIZE  (AVL)|\n");
    for (size_t coll_count = MIN_COLL_COUNT; coll_count < MAX_COLL_COUNT; coll_count *= COLL_STEP)
    {
        hash_searching_data[0] = ZERO_CHAR + coll_count;

        // ---

        open_hashstat_data_add(open_hash_table, open_hash_table->hash_function(hash_searching_data, open_hash_table->size), coll_count, hash_searching_data);
        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            open_hash_table_delete(open_hash_table, hash_searching_data);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            open_time_delete += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);

            str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
            strcpy(str, hash_searching_data);
            data_add(&open_hash_table->data[open_hash_table->hash_function(hash_searching_data, open_hash_table->size)], str, &compares);

            str_unpin(&str, &size);
            compares = 0;
        }
        open_hash_table_search(open_hash_table, hash_searching_data, &compares);
        open_time_delete_compares = compares;
        compares = 0;

        // ---

        close_hashstat_data_add(close_hash_table, close_hash_table->hash_function(hash_searching_data, close_hash_table->size), coll_count, hash_searching_data);
        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            close_hash_table_delete(close_hash_table, hash_searching_data);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            close_time_delete += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);

            str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
            strcpy(str, hash_searching_data);
            close_hash_table_data_add(close_hash_table, close_hash_table->hash_function(hash_searching_data, close_hash_table->size), str, &compares);

            str_unpin(&str, &size);
            compares = 0;
        }
        close_hash_table_search(close_hash_table, hash_searching_data, &compares);
        close_time_delete_compares = compares;
        compares = 0;

        // ---

        hashstat_data_str_create(avl_searching_data, 0);
        avl_data_add(&avl_tree, coll_count);
        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            avl_node_delete(&avl_tree, avl_searching_data);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            AVL_time_delete += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);

            str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
            strcpy(str, avl_searching_data);
            tmp = node_alloc(str);
            avl_tree = avl_node_add(avl_tree, tmp);

            str_unpin(&str, &size);
        }
        tmp = node_search(avl_tree, avl_searching_data, &compares);
        AVL_time_delete_compares = compares;
        compares = 0;

        printf("%-*zu|", STR_TABLE_SIZE + 1 - 2, coll_count);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, open_time_delete / MAX_ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, close_time_delete / MAX_ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, AVL_time_delete / MAX_ITER_COUNT);
        printf("%-*d|", STR_TABLE_SIZE * 2 - 2, open_time_delete_compares);
        printf("%-*d|", STR_TABLE_SIZE * 2 - 2, close_time_delete_compares);
        printf("%-*d|", STR_TABLE_SIZE * 2 - 2, AVL_time_delete_compares);
        printf("%-*zu|", STR_TABLE_SIZE * 2 - 2, open_hash_table_size(open_hash_table));
        printf("%-*zu|", STR_TABLE_SIZE * 2 - 2, close_hash_table_size(close_hash_table));
        printf("%-*zu|", STR_TABLE_SIZE * 2 - 2, sizeof(avl_tree) + node_size(avl_tree));

        printf("\n");

        open_time_delete = 0.0;
        close_time_delete = 0.0;

        open_time_delete_compares = 0;
        close_time_delete_compares = 0;

        for (size_t i = 0; i < open_hash_table->size; i++)
            open_hash_table_data_free(&open_hash_table->data[i]);
        
        close_hashstat_data_clear(close_hash_table);

        avl_tree = node_free(avl_tree);
    }

    open_hash_table_free(&open_hash_table);
    close_hash_table_free(&close_hash_table);
    avl_tree = node_free(avl_tree);
}






























void total_stat_search(void)
{
    char hash_searching_data[HASHSTAT_STR_SIZE] = "Searchable";
    char avl_searching_data[HASHSTAT_STR_SIZE] = {0};

    int compares = 0;

    node_t *worst_tree = NULL;
    node_t *max_depth_element = NULL;

    node_t *avl_tree = NULL;
    open_hash_table_t *open_hash_table = NULL;
    close_hash_table_t *close_hash_table = NULL;

    double time_tmp;

    // ---

    struct timespec t_beg, t_end;

    double WORTH_time_search = 0;
    int WORTH_time_search_compares = 0;

    double AVL_time_search = 0;
    int AVL_time_search_compares = 0;

    double open_time_search = 0;
    int open_time_search_compares = 0;

    double close_time_search = 0;
    int close_time_search_compares = 0;

    // ---

    open_hash_table = open_hash_table_init();
    if (! open_hash_table)
        return;
    
    close_hash_table = close_hash_table_init();
    if (! close_hash_table)
        return;

    // ---

    printf("\nTOTAL STATISTICS: SEARCH (time in nsec) (total iteration: %d)\n", ITER_COUNT);
    printf("(P.S. count is (collision count + 1) for hash tables)\n");

    printf("\nBINARY HASH FUNCTION; SIZE = %zu\n", open_hash_table->size);
    printf(" count |");
    printf("  WORTH TREE  |");
    printf("    AVL TREE  |");
    printf("   OPEN HASH  |");
    printf("  CLOSE HASH  |");
    printf(" COMP (WORST) |");
    printf(" COMP   (AVL) |");
    printf(" COMP  (OPEN) |");
    printf(" COMP (CLOSE) |\n");
    for (size_t coll_count = MIN_COLL_COUNT; coll_count < MAX_COLL_COUNT; coll_count *= COLL_STEP)
    {
        hash_searching_data[0] = ZERO_CHAR + coll_count;

        // ---

        open_hashstat_data_add(open_hash_table, open_hash_table->hash_function(hash_searching_data, open_hash_table->size), coll_count, hash_searching_data);

        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            open_hash_table_search(open_hash_table, hash_searching_data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            open_time_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            open_time_search_compares = compares;

            compares = 0;
        }

        for (size_t i = 0; i < open_hash_table->size; i++)
            open_hash_table_data_free(&open_hash_table->data[i]);

        // ---

        close_hashstat_data_add(close_hash_table, close_hash_table->hash_function(hash_searching_data, close_hash_table->size), coll_count, hash_searching_data);

        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            close_hash_table_search(close_hash_table, hash_searching_data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            close_time_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            close_time_search_compares = compares;

            compares = 0;
        }

        close_hashstat_data_clear(close_hash_table);

        // ---

        worst_tree = node_linked_list_tree(&max_depth_element, coll_count, &time_tmp);
        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            node_search(worst_tree, max_depth_element->data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            WORTH_time_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            WORTH_time_search_compares = compares;

            compares = 0;
        }

        // ---

        hashstat_data_str_create(avl_searching_data, 0);
        avl_data_add(&avl_tree, coll_count);
        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            node_search(avl_tree, avl_searching_data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            AVL_time_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            AVL_time_search_compares = compares;

            compares = 0;
        }

        printf("%-*zu|", STR_TABLE_SIZE + 1 - 2, coll_count);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, WORTH_time_search / MAX_ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, AVL_time_search / MAX_ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, open_time_search / MAX_ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, close_time_search / MAX_ITER_COUNT);
        printf("%-*d|", STR_TABLE_SIZE * 2 - 2, WORTH_time_search_compares);
        printf("%-*d|", STR_TABLE_SIZE * 2 - 2, AVL_time_search_compares);
        printf("%-*d|", STR_TABLE_SIZE * 2 - 2, open_time_search_compares);
        printf("%-*d|", STR_TABLE_SIZE * 2 - 2, close_time_search_compares);

        printf("\n");

        WORTH_time_search = 0.0;
        AVL_time_search = 0.0;
        open_time_search = 0.0;
        close_time_search = 0.0;

        WORTH_time_search_compares = 0;
        AVL_time_search_compares = 0;
        open_time_search_compares = 0;
        close_time_search_compares = 0;
    }

    open_hash_table_free(&open_hash_table);
    close_hash_table_free(&close_hash_table);

    node_free(avl_tree);
}