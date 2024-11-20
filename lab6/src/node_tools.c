#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "file_tools.h"
#include "node_tools.h"

#define EPS 1e-8

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

node_t *node_search(node_t *node, char *data)
{
    if (! node)
        return NULL;

    if (strcmp(data, node->data) == 0)
        return node;
    else if (strcmp(data, node->data) < 0)
        return node_search(node->left, data);
    else
        return node_search(node->right, data);
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

void node_ideal_create(node_t **node, int base_data, int height)
{
    node_t *tmp = NULL;
    char *data = NULL;

    if (! height)
    {
        *node = NULL;

        return;
    }

    data = malloc(sizeof(char) * decs(base_data) + sizeof(char));
    if (! data)
    {
        *node = NULL;

        return;
    }

    sprintf(data, "%d", base_data);

    tmp = node_alloc(data);
    if (! tmp)
    {
        free(data);
        *node = NULL;

        return;
    }

    *node = tmp;

    node_ideal_create(&tmp->left, *data - my_pow(2, height - 1), height - 1);
    node_ideal_create(&tmp->right, *data + my_pow(2, height - 1), height - 1);
}

node_t *node_ideal_tree(node_t **searched_element, int *tree_h, int elements)
{
    node_t *tmp = NULL;
    int required_height;

    required_height = (int)(log(elements + 1) / log(2)); 

    srand(time(NULL));

    node_ideal_create(&tmp, my_pow(2, required_height - 1), required_height);

    *searched_element = node_max_height_element(tmp);
    *tree_h = node_height(tmp);
    
    return tmp;
}

node_t *node_linked_list_tree(node_t **searched_element, int *tree_h, int elements)
{
    
}

//------------------------------


int node_statistics(char *filename, char c)
{
    node_t *tree = NULL;

    size_t tmp = 0;
    double time_tree_count_and_color = 0.0;
    double time_file_count_and_color = 0.0;

    struct timespec t_beg, t_end;

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

        if (node_search(root_tmp, word))
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
