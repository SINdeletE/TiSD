#include <stddef.h>
#include <string.h>

#include "file_tools.h"
#include "avl_tools.h"

int avl_node_height(node_t *node)
{
    if (! node)
        return -1;

    return node->height;
}

void node_height_set(node_t *node)
{
    int left_h = 0;
    int right_h = 0;

    left_h = avl_node_height(node->left);
    right_h = avl_node_height(node->right);

    node->height = (left_h > right_h ? left_h : right_h) + 1;
}

void node_height_fill(node_t *node)
{
    if (! node)
        return;

    node_height_fill(node->left);
    node_height_fill(node->right);

    node_height_set(node);
}

int node_height_diff(node_t *node)
{
    int left_h = 0;
    int right_h = 0;

    if (! node)
        return 0;

    left_h = avl_node_height(node->left);
    right_h = avl_node_height(node->right);

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

    node_height_set(node);
    node_height_set(tmp);


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

    node_height_set(node);
    node_height_set(tmp);


    return tmp;
}

node_t *node_big_rotate_left(node_t *node)
{
    node->right = node_rotate_right(node->right);
    node = node_rotate_left(node);

    return node;
}

node_t *node_big_rotate_right(node_t *node)
{
    node->left = node_rotate_left(node->left);
    node = node_rotate_right(node);

    return node;
}

node_t *avl_node_balance(node_t *node)
{
    if (! node)
        return node;

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
            avl_node_delete(&(*node)->left, data);
        }
        else if ((*node)->right)
        {
            tmp = node_right_min((*node)->right);

            node_data_swap(tmp, *node);
            avl_node_delete(&(*node)->right, data);
        }
        else
            *node = node_free(*node);
    }
    else if (strcmp((*node)->data, data) > 0)
        avl_node_delete(&(*node)->left, data);
    else
        avl_node_delete(&(*node)->right, data);

    *node = avl_node_balance(*node);
}

int tree_to_avl_cpy(node_t *node, node_t **avl_tree)
{
    node_t *tmp = NULL;
    int code = 0;

    if (! node)
        return 0;

    if (! (tmp = node_cpy(node)))
        return 1;

    *avl_tree = avl_node_add(*avl_tree, tmp);

    code += tree_to_avl_cpy(node->left, avl_tree);
    code += tree_to_avl_cpy(node->right, avl_tree);

    return code;
}
