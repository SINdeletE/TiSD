#include <stdlib.h>

#include "file_tools.h"
#include "node_tools.h"

node_t *node_free(node_t *node)
{
    if (! node)
        return NULL;

    if (node->left)
        node->left = node_free(node->left);

    if (node->right)
        node->fight = node_free(node->right);

    free(node->data);
    node->data = NULL;

    free(node);

    return NULL;
}

node_t *node_alloc(char *data)
{
    node_t *tmp = NULL;

    tmp = calloc(1, sizeof(list_t));
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
    list_t *tmp = NULL;

    if (! *node)
        return;

    if (strcmp((*node)->data, data) == 0)
    {
        if (node->left)
        {
            tmp = node_left_max(node->left);

            node_data_swap(tmp, *node);
            node_delete(&node->left, data);
        }
        else if (node->right)
        {
            tmp = node_right_min(node->right);

            node_data_swap(tmp, *node);
            node_delete(&node->right, data);
        }
        else
            *node = node_free(*node);
    }
    else
    {
        node_delete(&node->left, data);
        node_delete(&node->right, data);
    }
}

node_t *node_search(node_t *node, char *data)
{
    node_t *left = NULL;
    node_t *right = NULL;

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
    node_t *left = NULL;
    node_t *right = NULL;

    if (! node)
        return;
    
    fputs(node->data, f);
    fputs("\n", f);
    node_output_pre_order(node->left, f);
    node_output_pre_order(node->right, f);
}

void node_output_in_order(node_t *node, FILE *f)
{
    node_t *left = NULL;
    node_t *right = NULL;

    if (! node)
        return;
    
    node_output_in_order(node->left, f);
    fputs(node->data, f);
    fputs("\n", f);
    node_output_in_order(node->right, f);
}

void node_output_post_order(node_t *node, FILE *f)
{
    node_t *left = NULL;
    node_t *right = NULL;

    if (! node)
        return;
    
    node_output_post_order(node->left, f);
    node_output_post_order(node->right, f);
    fputs(node->data, f);
    fputs("\n", f);
}

int node_read_by_file(char *filename, node_t **root)
{
    FILE *f = NULL;
    int code;

    char *word = NULL;
    size_t size = 0;

    node_t *root_tmp = NULL;
    node_t *tmp = NULL;

    if (code = file_is_correct(filename))
        return code;

    f = fopen(filename, "r");

    while (getline(&word, &size, f) != -1)
    {
        tmp = node_alloc(word);
        if (! tmp)
        {
            fclose(f);
            free(word);
            node_free(root_tmp);

            return READ_ERR_INVALID_ALLOC;
        }

        root_tmp = node_add(root_tmp, tmp);

        word = NULL;
        size = 0;
    }

    if (! root_tmp)
        return READ_ERR_NO_DATA;

    fclose(f);

    *root = root_tmp;

    return READ_OK;
}
