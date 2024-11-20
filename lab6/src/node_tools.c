#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <ctype.h>

#include "file_tools.h"
#include "node_tools.h"

#define EPS 1e-8

#define STR_TABLE_SIZE 8

#define MAX_RAND_ELEM 8192
#define MIN_RAND_ELEM 0

#define MAX_RAND_SIZE 1024
#define MIN_RAND_SIZE 8

node_t *node_free(node_t *node)
{
    if (! node)
        return NULL;

    if (node->left)
        node->left = node_free(node->left);

    if (node->right)
        node->right = node_free(node->right);

    free(node->data);
    node->data = NULL;

    free(node);

    return NULL;
}

node_t *node_alloc(char *data)
{
    node_t *tmp = NULL;

    tmp = calloc(1, sizeof(node_t));
    if (! tmp)
        return tmp;
    
    tmp->data = data;

    return tmp;
}

void node_data_swap(node_t *node_a, node_t *node_b)
{
    char *tmp = NULL;
    color_t col;

    tmp = node_a->data;
    node_a->data = node_b->data;
    node_b->data = tmp;

    col = node_a->color;
    node_a->color = node_b->color;
    node_b->color = col;
}

node_t *node_add(node_t *node, node_t *elem)
{
    if (! node)
        return elem;

    if (strcmp(node->data, elem->data) == 0)
        return node;
    else if (strcmp(node->data, elem->data) < 0)
        node->right = node_add(node->right, elem);
    else
        node->left = node_add(node->left, elem);

    return node;
}

node_t *node_left_max(node_t *node)
{
    if (! node->right)
        return node;

    return node_left_max(node->right);
}

node_t *node_right_min(node_t *node)
{
    if (! node->left)
        return node;

    return node_right_min(node->left);
}

void node_delete(node_t **node, char *data)
{
    node_t *tmp = NULL;

    if (! *node)
        return;

    if (strcmp((*node)->data, data) == 0)
    {
        if ((*node)->left)
        {
            tmp = node_left_max((*node)->left);

            node_data_swap(tmp, *node);
            node_delete(&(*node)->left, data);
        }
        else if ((*node)->right)
        {
            tmp = node_right_min((*node)->right);

            node_data_swap(tmp, *node);
            node_delete(&(*node)->right, data);
        }
        else
            *node = node_free(*node);
    }
    else
    {
        node_delete(&(*node)->left, data);
        node_delete(&(*node)->right, data);
    }
}

node_t *node_search(node_t *node, char *data, int *compares)
{
    if (! node)
    {
        (*compares)++;

        return NULL;
    }

    if (strcmp(data, node->data) == 0)
    {
        (*compares) += 2;

        return node;
    }
    else if (strcmp(data, node->data) < 0)
    {
        (*compares) += 3;

        return node_search(node->left, data, compares);
    }
    
    (*compares) += 3;

    return node_search(node->right, data, compares);
}

void node_output_pre_order(node_t *node, FILE *f)
{
    if (! node)
        return;
    
    fputs(node->data, f);
    fputs("\n", f);
    node_output_pre_order(node->left, f);
    node_output_pre_order(node->right, f);
}

void node_output_in_order(node_t *node, FILE *f)
{
    if (! node)
        return;
    
    node_output_in_order(node->left, f);
    fputs(node->data, f);
    fputs("\n", f);
    node_output_in_order(node->right, f);
}

void node_output_post_order(node_t *node, FILE *f)
{
    if (! node)
        return;
    
    node_output_post_order(node->left, f);
    node_output_post_order(node->right, f);
    fputs(node->data, f);
    fputs("\n", f);
}

// ----------

static void node_print_dot_null(FILE *f, const char *data, int nullcount)
{
    // Описание "отсутствующей" вершины
    fprintf(f, "  null%d [shape=point];\n", nullcount);
    
    fprintf(f, "  \"");
    fputs(data, f);
    fprintf(f, "\" -> ");
    fprintf(f, "null%d;\n", nullcount);
}


static void node_print_dot_aux(FILE *f, node_t *node)
{
    // Счетчик "отсутсвующих" вершин (идентификатор)
    static int nullcount = 0;

    if (node->color == RED)
    {
        fprintf(f, "  \"");
        fputs(node->data, f);
        fprintf(f, "\"");
        fprintf(f, " [color=red];\n");
    }

    if (node->left)
    {
        fprintf(f, "  \"");
        fputs(node->data, f);
        fprintf(f, "\" -> \"");
        fputs(node->left->data, f);
        fprintf(f, "\";\n");
        node_print_dot_aux(f, node->left);
    }
    else
        node_print_dot_null(f, node->data, nullcount++);

    if (node->right)
    {
        fprintf(f, "  \"");
        fputs(node->data, f);
        fprintf(f, "\" -> \"");
        fputs(node->right->data, f);
        fprintf(f, "\";\n");
        node_print_dot_aux(f, node->right);
    }
    else
        node_print_dot_null(f, node->data, nullcount++);
}


void node_export_to_dot_eli(FILE *f, const char *node_data, node_t *node)
{
    fprintf(f, "digraph \"");
    fputs(node_data, f);
    fprintf(f, "\" {\n");
    fprintf(f, "  node [fontdata=\"Arial\"];\n");

    if (!node)
        fprintf(f, "\n");
    else if (!node->right && !node->left)
    {
        fprintf(f, "  \"");
        fputs(node->data, f);
        fprintf(f, "\";\n");
    }
    else
        node_print_dot_aux(f, node);

    fprintf(f, "}\n");
}

size_t node_count_and_color(node_t *head, char c)
{
    if (! head)
        return 0;

    if (*head->data == c)
    {
        head->color = RED;
    }

    return (*head->data == c) + node_count_and_color(head->left, c) + node_count_and_color(head->right, c);
}


// ------------------------------

double rand_chance(double l, double r)
{
    double random_num = (double)rand() / RAND_MAX;

    return (random_num * (r - l)) + l;
}

int rand_get(int l, int r)
{
    int random_num = rand();

    return random_num % (r - l) + l;
}

int max(int a, int b)
{
    if (a >= b)
        return a;
    
    return b;
}

int my_pow(int a, int n)
{
    int res = 1;

    for (int i = 0; i < n; i++)
        res *= a;

    return res;
}

int decs(int a)
{
    int s = 0;

    if (a == 0)
        return 1;

    while (a > 0)
    {
        a /= 10;

        s++;
    }

    return s;
}

int node_height(node_t *node)
{
    int max_h, left, right;

    if (! node)
        return -1;

    left = node_height(node->left);
    right = node_height(node->right);

    max_h = max(left, right);

    return max_h + 1;
}

node_t *node_max_height_element(node_t *node)
{
    int max_h, left, right;

    if (! node)
        return NULL;

    left = node_height(node->left);
    right = node_height(node->right);

    if (left == -1 && right == -1)
        return node;

    max_h = max(left, right);

    if (max_h == left)
        return node_max_height_element(node->left);
    else
        return node_max_height_element(node->right);
}

node_t *node_alloc_with_num(int num, int max_decs)
{
    node_t *tmp = NULL;
    char *data = NULL;
    char *p = NULL;

    data = calloc(max_decs + 1, sizeof(char));
    if (! data)
        return NULL;

    sprintf(data, "%*d", max_decs, num);
    p = data;
    while (isspace(*p))
    {
        *p = 'a' - 1;
        
        p++;
    }

    tmp = node_alloc(data);
    if (! tmp)
    {
        free(data);

        return NULL;
    }

    return tmp;
}

void node_ideal_create(node_t **node, int base_data, int max_decs, int height)
{
    node_t *tmp = NULL;

    if (! height)
    {
        *node = NULL;

        return;
    }

    tmp = node_alloc_with_num(base_data, max_decs);
    if (! tmp)
        return;

    *node = tmp;

    height--;
    node_ideal_create(&tmp->left, base_data - my_pow(2, height - 1), max_decs, height);
    node_ideal_create(&tmp->right, base_data + my_pow(2, height - 1), max_decs, height);
}

node_t *node_ideal_tree(node_t **searched_element, int elements)
{
    node_t *tmp = NULL;
    int required_height;

    required_height = (int)(log(elements + 1) / log(2)); 

    node_ideal_create(&tmp, my_pow(2, required_height - 1), decs(elements), required_height);

    *searched_element = node_max_height_element(tmp);
    
    return tmp;
}

node_t *node_linked_list_tree(node_t **searched_element, int elements)
{
    node_t *tree = NULL;
    node_t *tmp = NULL;

    size_t index = 0;
    char str[512] = {0};

    char *data = NULL;

    int i = 0;

    srand(time(NULL));

    str[index] = 'a';
    while (i < elements)
    {
        if (str[index] == 'z')
        {
            index++;

            str[index] = 'a';
        }
        else
            str[index]++;

        data = calloc((strlen(str) + 1), sizeof(char));
        strcpy(data, str);

        tmp = node_alloc(data);
        if (! tmp)
        {
            free(data);

            break;
        }

        tree = node_add(tree, tmp);

        i++;
    }

    *searched_element = node_max_height_element(tmp);
    
    return tree;
}

//------------------------------

int node_statistics(char *filename, char c)
{
    FILE *f = NULL;
    char graphname[256] = "";
    char pngname[256] = "";
    char command[2024] = "";

    node_t *tree = NULL;
    node_t *tree_tmp = NULL;
    int tmp = 0;

    double time_tree_count_and_color = 0.0;
    double time_file_count_and_color = 0.0;

    // -----

    double time_ideal_tree_sort = 0.0;
    double time_linked_list_tree_sort = 0.0;

    node_t *max_depth_element = NULL;
    double time_ideal_tree_search = 0.0;
    double time_linked_list_tree_search = 0.0;

    int compares = 0;
    int total_ideal_tree_compares = 0;
    int total_linked_list_tree_compares = 0;

    // -----

    struct timespec t_beg, t_end;

    printf("\nSTATISTICS (time in nsec)\n");
    printf("n count |");
    printf("h IDEAL |");
    printf("h WORST |");
    printf("TREE SORT(IDEAL)|");
    printf("TREE SORT(WORST)|");
    printf("TREE FIND(IDEAL)|");
    printf("TREE FIND(WORST)|");
    printf("COMPARES (IDEAL)|");
    printf("COMPARES (WORST)|");
    printf("  best t (SORT) |");
    printf("  best t (FIND) |\n");

    for (int n = 4; n <= MAX_ELEMS_COUNT; n *= 2)
    {
        for (int i = 0; i < MAX_ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            tree = node_ideal_tree(&max_depth_element, n);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            tree = node_free(tree);

            time_ideal_tree_sort += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
        }

        tree = node_ideal_tree(&max_depth_element, n);

        sprintf(graphname, "Graph_ideal%d.gv", n);
        sprintf(pngname, "Graph_ideal%d.png", n);
        
        sprintf(command, "dot -Tpng ");
        strcat(command, graphname);
        strcat(command, " -o ");
        strcat(command, pngname);

        f = fopen(graphname, "w");
        node_export_to_dot_eli(f, "graph", tree);
        fclose(f);
        system(command);

        for (int i = 0; i < MAX_ITER_COUNT; i++)
        {
            compares = 0;
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            tree_tmp = node_search(tree, max_depth_element->data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            time_ideal_tree_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            total_ideal_tree_compares = compares;
        }

        tree = node_free(tree);

        for (int i = 0; i < MAX_ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            tree = node_linked_list_tree(&max_depth_element, n);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            tree = node_free(tree);

            time_linked_list_tree_sort += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
        }

        tree = node_linked_list_tree(&max_depth_element, n);

        sprintf(graphname, "Graph_linked_list%d.gv", n);
        sprintf(pngname, "Graph_linked_list%d.png", n);

        sprintf(command, "dot -Tpng ");
        strcat(command, graphname);
        strcat(command, " -o ");
        strcat(command, pngname);

        f = fopen(graphname, "w");
        node_export_to_dot_eli(f, "graph", tree);
        fclose(f);
        system(command);

        for (int i = 0; i < MAX_ITER_COUNT; i++)
        {
            compares = 0;
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            tree_tmp = node_search(tree, max_depth_element->data, &compares);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

            time_linked_list_tree_search += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            total_linked_list_tree_compares = compares;
        }

        tree = node_free(tree);

        tree_tmp++;

        printf("%-*d|", STR_TABLE_SIZE, n - 1);
        printf("%-*d|", STR_TABLE_SIZE, (int)(log(n - 1) / log(2)));
        printf("%-*d|", STR_TABLE_SIZE, n - 1);
        printf("%-*lf|", STR_TABLE_SIZE * 2, time_ideal_tree_sort / MAX_ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2, time_linked_list_tree_sort / MAX_ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2, time_ideal_tree_search / MAX_ITER_COUNT);
        printf("%-*lf|", STR_TABLE_SIZE * 2, time_linked_list_tree_search / MAX_ITER_COUNT);
        printf("%-*d|", STR_TABLE_SIZE * 2, total_ideal_tree_compares);
        printf("%-*d|", STR_TABLE_SIZE * 2, total_linked_list_tree_compares);
        if (time_ideal_tree_sort < time_linked_list_tree_sort)
            printf("      IDEAL     |");
        else if (time_ideal_tree_sort > time_linked_list_tree_sort)
            printf("      WORST     |");
        else 
            printf("      EQUAL     |");

        if (time_ideal_tree_search < time_linked_list_tree_search)
            printf("      IDEAL     |");
        else if (time_ideal_tree_search > time_linked_list_tree_search)
            printf("      WORST     |");
        else 
            printf("      EQUAL     |");

        printf("\n");
    }

    if (file_is_correct(filename))
        return STAT_ERR_INVALID_FILE;

    if (node_read_by_file(filename, &tree))
        return STAT_ERR_INVALID_READ;

    for (size_t i = 0; i < MAX_ITER_COUNT; i++)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        tmp = node_count_and_color(tree, c);
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

        time_tree_count_and_color += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
    }

    for (size_t i = 0; i < MAX_ITER_COUNT; i++)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        tmp = file_search_by_char(filename, c);
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

        time_file_count_and_color += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
    }

    tmp = tmp + 1; // Чтобы компилятор не ругался

    printf("\n");
    printf("Average tree search: %.6lf\n", time_tree_count_and_color / MAX_ITER_COUNT);
    printf("Average file search: %.6lf\n", time_file_count_and_color / MAX_ITER_COUNT);

    if (time_tree_count_and_color - time_file_count_and_color < -EPS)
        printf("tree method is faster than file method on %.6lf%%\n", time_file_count_and_color / time_tree_count_and_color * 100.0);
    else if (time_tree_count_and_color - time_file_count_and_color > EPS)
        printf("file method is faster than tree method on %.6lf%%\n", time_tree_count_and_color / time_file_count_and_color * 100.0);
    else
        printf("file method and tree method has same times");

    tree = node_free(tree);

    return STAT_OK;
}

// ----------

int node_read_by_file(char *filedata, node_t **root)
{
    FILE *f = NULL;
    int code;

    char *word = NULL;
    size_t size = 0;
    
    char *word_tmp = NULL;

    node_t *root_tmp = NULL;
    node_t *tmp = NULL;
    int comp = 0;

    if ((code = file_is_correct(filedata)))
        return READ_ERR_INVALID_FILE;

    f = fopen(filedata, "r");

    while (getline(&word, &size, f) != -1)
    {
        tmp = node_alloc(word);
        if (! tmp)
        {
            fclose(f);
            str_free(&word, &size);
            node_free(root_tmp);

            return READ_ERR_INVALID_ALLOC;
        }

        if ((word_tmp = strchr(word, '\n')))
        {
            *word_tmp = '\0';
            word_tmp = NULL;
        }

        tmp->data = word;

        if (node_search(root_tmp, word, &comp))
            node_free(tmp);
        else
        {
            root_tmp = node_add(root_tmp, tmp);
        }

        str_unpin(&word, &size);
    }

    str_free(&word, &size);

    if (! root_tmp)
        return READ_ERR_NO_DATA;

    fclose(f);

    *root = root_tmp;

    return READ_OK;
}
