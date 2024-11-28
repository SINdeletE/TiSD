#include <stddef.h>
#include <string.h>

#include "avl_tools.h"

int node_height(node_t *node)
{
    if (! node)
        return 0;

    return node->height;
}

void node_height_set(node_t *node)
{
    int left_h = 0;
    int right_h = 0;

    left_h = node_height(node->left);
    right_h = node_height(node->right);

    node->height = (left_h > right_h ? left_h : right_h) + 1;
}

int node_height_fill(node_t *node)
{
    int left = 0;
    int right = 0;

    if (! node)
        return 0;

    left = node_height_fill(node->left) + 1;
    right = node_height_fill(node->right) + 1;

    node_height_set(node);
}

int node_height_diff(node_t *node)
{
    int left_h = 0;
    int right_h = 0;

    if (! node)
        return 0;

    left_h = node_height(node->left);
    right_h = node_height(node->right);

    return left_h - right_h;
}

node_t *node_rotate_left(node_t *node)
{
    node_t *tmp = NULL;

    if (! node)
        return node;

    tmp = node->right;
    node->right = tmp->left;
    tmp->left = node;

    node->height = node_height_set(node);
    tmp->height = node_height_set(tmp);


    return tmp;
}

node_t *node_rotate_right(node_t *node)
{
    node_t *tmp = NULL;

    if (! node)
        return node;

    tmp = node->left;
    node->left = tmp->right;
    tmp->right = node;

    node->height = node_height_set(node);
    tmp->height = node_height_set(tmp);


    return tmp;
}

node_t node_big_rotate_left(node_t *node)
{
    node->right = node_rotate_right(node->right);
    node = node_rotate_left(node);

    return node;
}

node_t node_big_rotate_right(node_t *node)
{
    node->left = node_rotate_left(node->left);
    node = node_rotate_right(node);

    return node;
}

node_t *avl_node_balance(node_t *node)
{
    node_height_set(node);
    if (node_height_diff(node) == -2)
    {
        if (node_height_diff(node->right) == 1) 
            node = node_big_rotate_left(node);
        else
            node = node_rotate_left(node);
    }

    if (node_height_diff(node) == 2)
    {
        if (node_height_diff(node->left) == -1) 
            node = node_big_rotate_right(node);
        else
            node = node_rotate_right(node);
    }

    return node;
}

node_t *avl_node_add(node_t *node, node_t *elem)
{
    if (! node)
        return elem;

    if (strcmp(node->data, elem->data) == 0)
        return node;
    else if (strcmp(node->data, elem->data) < 0)
        node->right = avl_node_add(node->right, elem);
    else
        node->left = avl_node_add(node->left, elem);

    return avl_node_balance(node);
}

void avl_node_delete(node_t **node, char *data)
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

    *node = avl_node_balance(*node);
}

int avl_node_read_by_file(char *filedata, node_t **root)
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
            root_tmp = avl_node_add(root_tmp, tmp);
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
