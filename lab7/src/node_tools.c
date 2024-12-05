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
    // color_t col;

    tmp = node_a->data;
    node_a->data = node_b->data;
    node_b->data = tmp;

    // col = node_a->color;
    // node_a->color = node_b->color;
    // node_b->color = col;
}

int node_height(node_t *node)
{
    int max_h, left, right;

    if (! node)
        return -1;

    left = node_height(node->left);
    right = node_height(node->right);

    max_h = left > right ? left : right;

    return max_h + 1;
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
        return NULL;

    (*compares)++;

    if (strcmp(data, node->data) == 0)
        return node;
    else if (strcmp(data, node->data) > 0)
        return node_search(node->right, data, compares);

    return node_search(node->left, data, compares);
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

    // if (node->color == RED)
    // {
    //     fprintf(f, "  \"");
    //     fputs(node->data, f);
    //     fprintf(f, "\"");
    //     fprintf(f, " [color=red];\n");
    // }

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

void node_delete_by_char(node_t **node, char c)
{
    if (! node || ! *node)
        return;

    node_delete_by_char(&(*node)->left, c);
    node_delete_by_char(&(*node)->right, c);

    if (*(*node)->data == c)
        node_delete(node, (*node)->data);
}

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
    fclose(f);

    if (! root_tmp)
        return READ_ERR_NO_DATA;

    *root = root_tmp;

    return READ_OK;
}

// -----------------------------------

node_t *node_cpy(node_t *node)
{
    node_t *tmp = NULL;
    char *data_tmp = NULL;

    tmp = calloc(1, sizeof(node_t));
    if (! tmp)
        return NULL;

    data_tmp = calloc((strlen(node->data) + 1), sizeof(char));
    if (! data_tmp)
    {
        node_free(tmp);

        return NULL;
    }

    strcpy(data_tmp, node->data);

    tmp->data = data_tmp;

    return tmp;
}

size_t node_size(node_t *node)
{
    size_t total_size = 0;

    if (! node)
        return 0;

    total_size += sizeof(node_t);
    total_size += (strlen(node->data) + 1) * sizeof(char);
    total_size += node_size(node->left);
    total_size += node_size(node->right);

    return total_size;
}

size_t node_compares(node_t *node, node_t *root)
{
    size_t total_comp = 0;
    int comp = 0;

    if (! node || ! root)
        return 0;

    total_comp += node_compares(node->left, root);
    total_comp += node_compares(node->right, root);
    
    node_search(root, node->data, &comp);

    return total_comp + comp;
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

node_t *node_max_height_element(node_t *node)
{
    int max_h, left, right;

    if (! node)
        return NULL;

    left = node_height(node->left);
    right = node_height(node->right);

    if (left == -1 && right == -1)
        return node;

    max_h = left > right ? left : right;

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
        *p = '0';
        
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

node_t *node_linked_list_tree(node_t **searched_element, int elements, double *time)
{
    node_t *tree = NULL;
    node_t *tmp = NULL;

    int max_decs;
    int i = 0;

    struct timespec t_beg, t_end;

    max_decs = decs(elements);

    elements--;
    while (i <= elements)
    {
        tmp = node_alloc_with_num(i, max_decs);
        if (! tmp)
            break;

        // ---

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        tree = node_add(tree, tmp);
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        *time += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);

        // ---

        i++;
    }

    *searched_element = node_max_height_element(tmp);
    
    return tree;
}
