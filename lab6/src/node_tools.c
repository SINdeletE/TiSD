#include <stdlib.h>
#include <string.h>

#include "file_tools.h"
#include "node_tools.h"

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

    tmp = node_a->data;
    node_a->data = node_b->data;
    node_b->data = tmp;
}

node_t *node_add(node_t *node, node_t *elem)
{
    if (! node)
        return elem;

    if (strcmp(node->data, elem->data) == 0)
        return node;
    else if (strcmp(node->data, elem->data) < 0)
        node->left = node_add(node->left, elem);
    else
        node->right = node_add(node->right, elem);

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
    
    fprintf(f, "  %s -> null%d;\n", data, nullcount);
}


static void node_print_dot_aux(FILE *f, node_t *node)
{
    // Счетчик "отсутсвующих" вершин (идентификатор)
    static int nullcount = 0;

    if (node->right)
    {
        fprintf(f, "  %s -> %s;\n", node->data, node->right->data);
        node_print_dot_aux(f, node->right);
    }
    else
        node_print_dot_null(f, node->data, nullcount++);

    if (node->left)
    {
        fprintf(f, "  %s -> %s;\n", node->data, node->left->data);
        node_print_dot_aux(f, node->left);
    }
    else
        node_print_dot_null(f, node->data, nullcount++);
}


void node_export_to_dot_eli(FILE *f, const char *node_data, node_t *node)
{
    fprintf(f, "digraph %s {\n", node_data);
    fprintf(f, "  node [fontdata=\"Arial\"];\n");

    if (!node)
        fprintf(f, "\n");
    else if (!node->right && !node->left)
        fprintf(f, "  %s;\n", node->data);
    else
        node_print_dot_aux(f, node);

    fprintf(f, "}\n");
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
        root_tmp = node_add(root_tmp, tmp);

        word = NULL;
        size = 0;
    }

    str_free(&word, &size);

    if (! root_tmp)
        return READ_ERR_NO_DATA;

    fclose(f);

    *root = root_tmp;

    return READ_OK;
}
