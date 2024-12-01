#ifndef AVL_TOOLS_H__

#define AVL_TOOLS_H__

#include "node_tools.h"

node_t *avl_node_balance(node_t *node);
node_t *avl_node_add(node_t *node, node_t *elem);
void avl_node_delete(node_t **node, char *data);
void avl_node_delete_by_char(node_t **node, char c);

int tree_to_avl_cpy(node_t *node, node_t **avl_tree);

#endif
