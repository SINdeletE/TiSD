#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>
#include <limits.h>

#include "statistics.h"

#define MIN_INDEX 0
#define MAX_INDEX 5

#define ZERO_CHAR '0'
#define DEC_CONST 10

#define STR_TABLE_SIZE 8

double node_search_time(node_t *node, node_t *tree);
double open_hash_table_search_time(open_hash_table_t *hash_table);
double close_hash_table_search_time(close_hash_table_t *hash_table);

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
        if (close_hash_table_data_add(hash_table, hash, str, &comp))
            printf("LOKKKKKKKKKKKKKKK\n\n");

        str_unpin(&str, &size);
    }

    str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
    strcpy(str, last_str);
    if (close_hash_table_data_add(hash_table, hash, str, &comp))
        printf("FEWFKEP{WKF{WEFW}}\n\n");

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







// ---







void str_rand(char *str, size_t size)
{
    char r_num = 1;

    for (size_t i = 0; i < size; i++)
    {
        while (! (r_num = rand() % (CHAR_MAX - 2) + 1));
        str[i] = r_num;
    }
}

double open_rand_average_compare(size_t count, double *time)
{
    open_hash_table_t *hash_table = NULL;

    char *str = NULL;
    int comp = 0;
    size_t size = 0;

    double total_comp = 0.0;

    hash_table = open_hash_table_init();

    for (size_t i = 0; i < count; i++)
    {
        str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
        
        str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);
        while (open_hash_table_add(hash_table, str, &comp))
        {
            str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);

            comp = 0;
        }
        
        if (open_hash_compares(hash_table) - hash_table->comp_limit > -EPS && hash_table->size != TABLE_MAX_SIZE)
            open_hash_table_restruct(&hash_table, open_hash_new_size(hash_table), hash_table->hash_function);

        str_unpin(&str, &size);

        total_comp += comp;
        comp = 0;
    }

    while (open_hash_compares(hash_table) - hash_table->comp_limit > -EPS && hash_table->size != TABLE_MAX_SIZE) // РЕШЕНИЕ, ЕСЛИ РЕБАЛАНСИРОВКА НЕ ПОМОГЛА
        open_hash_table_restruct(&hash_table, open_hash_new_size(hash_table), hash_table->hash_function);

    total_comp = open_hash_compares(hash_table);
    *time += open_hash_table_search_time(hash_table);

    open_hash_table_free(&hash_table);

    return total_comp;
}

double close_rand_average_compare(size_t count, double *time)
{
    close_hash_table_t *hash_table = NULL;
    int code;

    char *str = NULL;
    int comp = 0;
    size_t size = 0;

    double total_comp = 0.0;

    hash_table = close_hash_table_init();

    for (size_t i = 0; i < (count > TABLE_INIT_SIZE ? TABLE_INIT_SIZE : count); i++)
    {
        str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
        str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);
        while ((code = close_hash_table_add(hash_table, str, &comp)))
        {
            if (code == HASH_PRCS_ERR_MAX_SIZE)
                close_hash_table_restruct(&hash_table, close_hash_new_size(hash_table), hash_table->hash_function);
            else
            {
                str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);

                comp = 0;
            }
        }

        if (close_hash_compares(hash_table) - hash_table->comp_limit > -EPS && hash_table->size != TABLE_MAX_SIZE)
            close_hash_table_restruct(&hash_table, close_hash_new_size(hash_table), hash_table->hash_function);

        str_unpin(&str, &size);

        total_comp += comp;
        comp = 0;
    }

    while (close_hash_compares(hash_table) - hash_table->comp_limit > -EPS && hash_table->size != TABLE_MAX_SIZE) // РЕШЕНИЕ, ЕСЛИ РЕБАЛАНСИРОВКА НЕ ПОМОГЛА
        close_hash_table_restruct(&hash_table, close_hash_new_size(hash_table), hash_table->hash_function);

    total_comp = close_hash_compares(hash_table);
    *time += close_hash_table_search_time(hash_table);

    close_hashstat_data_clear(hash_table);
    close_hash_table_free(&hash_table);

    return total_comp;
}

double node_rand_average_compare(size_t count, double *time)
{
    node_t *tree = NULL;
    node_t *tmp = NULL;

    char *str = NULL;
    size_t size = 0;

    double total_comp = 0.0;

    int cmp = 0;

    for (size_t i = 0; i < count; i++)
    {
        str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
        str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);
        while (node_search(tree, str, &cmp))
            str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);

        tmp = node_alloc(str);
        tree = node_add(tree, tmp);

        str_unpin(&str, &size);
    }

    total_comp = node_compares(tree, tree);
    *time += node_search_time(tree, tree);

    tree = node_free(tree);

    return total_comp / count;
}

double avl_node_rand_average_compare(size_t count, double *time)
{
    node_t *tree = NULL;
    node_t *tmp = NULL;

    char *str = NULL;
    size_t size = 0;

    double total_comp = 0.0;

    int cmp = 0;

    for (size_t i = 0; i < count; i++)
    {
        str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
        str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);
        while (node_search(tree, str, &cmp))
            str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);

        tmp = node_alloc(str);
        tree = avl_node_add(tree, tmp);

        str_unpin(&str, &size);
    }

    total_comp = node_compares(tree, tree);
    *time += node_search_time(tree, tree);

    tree = node_free(tree);

    return total_comp / count;
}





// ---







double open_rand_delete_time(size_t count)
{
    double total_time = 0.0;

    open_hash_table_t *hash_table = NULL;
    data_t *cur = NULL;

    char *str = NULL;
    int comp = 0;
    size_t size = 0;

    struct timespec t_beg, t_end;

    hash_table = open_hash_table_init();

    for (size_t i = 0; i < count; i++)
    {
        str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
        
        str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);
        while (open_hash_table_add(hash_table, str, &comp))
        {
            str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);

            comp = 0;
        }
        
        if (open_hash_compares(hash_table) - hash_table->comp_limit > -EPS && hash_table->size != TABLE_MAX_SIZE)
            open_hash_table_restruct(&hash_table, open_hash_new_size(hash_table), hash_table->hash_function);

        str_unpin(&str, &size);

        comp = 0;
    }

    while (open_hash_compares(hash_table) - hash_table->comp_limit > -EPS && hash_table->size != TABLE_MAX_SIZE) // РЕШЕНИЕ, ЕСЛИ РЕБАЛАНСИРОВКА НЕ ПОМОГЛА
        open_hash_table_restruct(&hash_table, open_hash_new_size(hash_table), hash_table->hash_function);

    for (size_t i = hash_table->size; i > 0; i--)
        if (hash_table->data[i - 1])
            while (hash_table->data[i - 1])
            {
                for (size_t z = 0; z < ITER_COUNT; z++)
                {
                    for (cur = hash_table->data[i - 1]; cur->next; cur = cur->next);

                    str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
                    strcpy(str, hash_table->data[i - 1]->str);

                    clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
                    open_hash_table_delete(hash_table, str);
                    clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

                    total_time += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);

                    open_hash_table_add(hash_table, str, &comp);

                    str_unpin(&str, &size);
                }

                for (cur = hash_table->data[i - 1]; cur->next; cur = cur->next);
                open_hash_table_delete(hash_table, cur->str);
            }

    open_hash_table_free(&hash_table);

    return total_time;
}

double close_rand_delete_time(size_t count)
{
    double total_time = 0.0;

    close_hash_table_t *hash_table = NULL;
    int code;

    char *str = NULL;
    int comp = 0;
    size_t size = 0;

    struct timespec t_beg, t_end;

    hash_table = close_hash_table_init();

    for (size_t i = 0; i < (count > TABLE_INIT_SIZE ? TABLE_INIT_SIZE : count); i++)
    {
        str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
        str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);
        while ((code = close_hash_table_add(hash_table, str, &comp)))
        {
            if (code == HASH_PRCS_ERR_MAX_SIZE)
                close_hash_table_restruct(&hash_table, close_hash_new_size(hash_table), hash_table->hash_function);
            else
            {
                str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);

                comp = 0;
            }
        }

        if (close_hash_compares(hash_table) - hash_table->comp_limit > -EPS && hash_table->size != TABLE_MAX_SIZE)
            close_hash_table_restruct(&hash_table, close_hash_new_size(hash_table), hash_table->hash_function);

        str_unpin(&str, &size);

        comp = 0;
    }

    while (close_hash_compares(hash_table) - hash_table->comp_limit > -EPS && hash_table->size != TABLE_MAX_SIZE) // РЕШЕНИЕ, ЕСЛИ РЕБАЛАНСИРОВКА НЕ ПОМОГЛА
        close_hash_table_restruct(&hash_table, close_hash_new_size(hash_table), hash_table->hash_function);

    for (size_t i = hash_table->size; i > 0; i--)
        if (hash_table->data[i - 1])
        {
            for (size_t z = 0; z < ITER_COUNT; z++)
            {
                str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
                strcpy(str, hash_table->data[i - 1]);

                clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
                close_hash_table_delete(hash_table, str);
                clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

                total_time += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);

                close_hash_table_add(hash_table, str, &comp);

                str_unpin(&str, &size);
            }

            close_hash_table_delete(hash_table, hash_table->data[i - 1]);
            
            str_free(&str, &size);
        }

    close_hashstat_data_clear(hash_table);
    close_hash_table_free(&hash_table);

    return total_time;
}

double avl_rec_time(node_t **node, node_t **tree)
{
    double total_time = 0.0;

    node_t *tmp = NULL;
    char *str = NULL;

    struct timespec t_beg, t_end;

    if (! node || ! *node || ! tree || ! *tree)
        return 0.0;

    if ((*node)->left)
        total_time += avl_rec_time(&(*node)->left, tree);
    if ((*node)->right)
        total_time += avl_rec_time(&(*node)->right, tree);

    for (size_t i = 0; i < ITER_COUNT; i++)
    {
        str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
        strcpy(str, (*node)->data);

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        node_delete(tree, str);
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

        total_time += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);

        tmp = node_alloc(str);
        *tree = node_add(*tree, tmp);
    }

    str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
    strcpy(str, (*node)->data);
    node_delete(tree, str);
    free(str);

    return total_time;
}

double avl_rand_delete_time(size_t count)
{
    node_t *tree = NULL;
    node_t *tmp = NULL;

    char *str = NULL;
    size_t size = 0;

    double total_time;

    int cmp = 0;

    for (size_t i = 0; i < count; i++)
    {
        str = calloc(HASHSTAT_STR_SIZE, sizeof(char));
        str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);
        while (node_search(tree, str, &cmp))
            str_rand(str, rand() % (HASHSTAT_STR_SIZE - 3) + 2);

        tmp = node_alloc(str);
        tree = avl_node_add(tree, tmp);

        str_unpin(&str, &size);
    }

    total_time = avl_rec_time(&tree, &tree);

    tree = node_free(tree);

    return total_time;
}


// ---







double node_search_time(node_t *node, node_t *tree)
{
    double total_time = 0.0;
    int comps = 0;

    struct timespec t_beg, t_end;

    if (! node)
        return 0;

    total_time += node_search_time(node->left, tree);
    total_time += node_search_time(node->right, tree);
    
    for (size_t i = 0; i < ITER_COUNT; i++)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        node_search(tree, node->data, &comps);
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

        total_time += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
    }

    return total_time;
}

double open_hash_table_search_time(open_hash_table_t *hash_table)
{
    double total_time = 0.0;
    int comps = 0;

    struct timespec t_beg, t_end;

    for (size_t i = 0; i < hash_table->size; i++)
        if (hash_table->data[i])
            for (data_t *cur = hash_table->data[i]; cur; cur = cur->next)
                for (size_t z = 0; z < ITER_COUNT; z++)
                {
                    clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
                    open_hash_table_search(hash_table, cur->str, &comps);
                    clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

                    total_time += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
                }
    
    return total_time;
}

double close_hash_table_search_time(close_hash_table_t *hash_table)
{
    double total_time = 0.0;
    int comps = 0;

    struct timespec t_beg, t_end;

    for (size_t i = 0; i < hash_table->size; i++)
        if (hash_table->data[i])
            for (size_t z = 0; z < ITER_COUNT; z++)
            {
                clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
                close_hash_table_search(hash_table, hash_table->data[i], &comps);
                clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

                total_time += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            }
    
    return total_time;
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
    close_hash_table_restruct(&close_hash_table, HASHES_MAX_COLL_COUNT + 1, binary_poly_hash_function);

    // ---

    printf("\nHASHES STATISTICS (time in nsec) (total iteration: %d)\n", ITER_COUNT);

    printf("\nBINARY HASH FUNCTION\n");
    printf("collision|");
    printf("HASH FIND (OPEN)|");
    printf("HASH FIND(CLOSE)|");
    printf("COMPARES  (OPEN)|");
    printf("COMPARES (CLOSE)|");
    printf("   best time    |\n");
    for (size_t coll_count = HASHES_MIN_COLL_COUNT; coll_count < HASHES_MAX_COLL_COUNT; coll_count += HASHES_COLL_STEP)
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
        printf("%-*lf|", STR_TABLE_SIZE * 2, open_time_search / ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2, close_time_search / ITER_COUNT);
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



void total_stat_size(void)
{
    char hash_searching_data[HASHSTAT_STR_SIZE] = "Searchable";

    node_t *worst_tree = NULL;
    node_t *max_depth_element = NULL;

    node_t *avl_tree = NULL;
    open_hash_table_t *open_hash_table = NULL;
    close_hash_table_t *close_hash_table = NULL;

    double time_tmp;

    open_hash_table = open_hash_table_init();
    if (! open_hash_table)
        return;
    
    close_hash_table = close_hash_table_init();
    if (! close_hash_table)
        return;
    close_hash_table_restruct(&close_hash_table, CLOSE_HASH_STAT_INIT, close_hash_table->hash_function);

    printf("\nTOTAL STATISTICS: SIZES (size in bytes)\n");

    printf(" count |");
    printf("  USUAL SIZE  |");
    printf("    AVL SIZE  |");
    printf("   OPEN SIZE  |");
    printf("  CLOSE SIZE  |\n");
    for (size_t coll_count = MIN_COLL_COUNT; coll_count < MAX_COLL_COUNT; coll_count *= COLL_STEP)
    {
        hash_searching_data[0] = ZERO_CHAR + coll_count;

        // ---

        open_hashstat_data_add(open_hash_table, open_hash_table->hash_function(hash_searching_data, open_hash_table->size), coll_count, hash_searching_data);

        // ---

        close_hashstat_data_add(close_hash_table, close_hash_table->hash_function(hash_searching_data, close_hash_table->size), coll_count, hash_searching_data);

        // ---

        worst_tree = node_linked_list_tree(&max_depth_element, coll_count, &time_tmp);

        // ---

        avl_data_add(&avl_tree, coll_count);

        printf("%-*zu|", STR_TABLE_SIZE + 1 - 2, coll_count);
        printf("%-*zu|", STR_TABLE_SIZE * 2 - 2, sizeof(worst_tree) + node_size(worst_tree));
        printf("%-*zu|", STR_TABLE_SIZE * 2 - 2, sizeof(avl_tree) + node_size(avl_tree));
        printf("%-*zu|", STR_TABLE_SIZE * 2 - 2, open_hash_table_size(open_hash_table));
        printf("%-*zu|", STR_TABLE_SIZE * 2 - 2, close_hash_table_size(close_hash_table));

        printf("\n");

        for (size_t i = 0; i < open_hash_table->size; i++)
            open_hash_table_data_free(&open_hash_table->data[i]);
        close_hashstat_data_clear(close_hash_table);
        worst_tree = node_free(worst_tree);
        avl_tree = node_free(avl_tree);
    }

    open_hash_table_free(&open_hash_table);
    close_hash_table_free(&close_hash_table);
}









void total_stat(void)
{
    double usual_time_search = 0;
    double avg_time_search = 0;
    double open_time_search = 0;
    double close_time_search = 0;

    printf("\nTOTAL STATISTICS: SEARCH AVERAGE COMPARES FROM RANDOM DATA (time in nsec) (total iteration: %d) (comp limit = %d)\n", ITER_COUNT, TABLE_INIT_COMP_LIMIT);
    printf("BINARY HASH FUNCTION\n");
    printf(" count |");
    printf("  USUAL COMP  |");
    printf("    AVL COMP  |");
    printf("   OPEN COMP  |");
    printf("  CLOSE COMP  |");
    printf("  USUAL TIME  |");
    printf("    AVL TIME  |");
    printf("   OPEN TIME  |");
    printf("  CLOSE TIME  |\n");

    srand(time(NULL));

    for (size_t i = MIN_COLL_COUNT; i < MAX_COLL_COUNT; i *= 2)
    {
        printf("%-*zu|", STR_TABLE_SIZE + 1 - 2, i);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, node_rand_average_compare(i, &usual_time_search));
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, avl_node_rand_average_compare(i, &avg_time_search));
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, open_rand_average_compare(i, &open_time_search));
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, close_rand_average_compare(i, &close_time_search));
        
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, usual_time_search / i / ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, avg_time_search / i / ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, open_time_search / i / ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, close_time_search / i / ITER_COUNT);

        printf("\n");

        usual_time_search = 0;
        avg_time_search = 0;
        open_time_search = 0;
        close_time_search = 0;
    }

    printf("\nTOTAL STATISTICS: DELETE AVERAGE COMPARES FROM RANDOM DATA (time in nsec) (total iteration: %d) (comp limit = %d)\n", ITER_COUNT, TABLE_INIT_COMP_LIMIT);
    printf("BINARY HASH FUNCTION\n");
    printf(" count |");
    printf("    AVL TIME  |");
    printf("   OPEN TIME  |");
    printf("  CLOSE TIME  |\n");

    for (size_t i = MIN_COLL_COUNT; i < MAX_COLL_COUNT; i *= 2)
    {
        printf("%-*zu|", STR_TABLE_SIZE + 1 - 2, i);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, avl_rand_delete_time(i) / i / ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, open_rand_delete_time(i) / i / ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2 - 2, close_rand_delete_time(i) / i / ITER_COUNT);

        printf("\n");

        avg_time_search = 0;
        open_time_search = 0;
        close_time_search = 0;
    }
}