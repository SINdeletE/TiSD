#ifndef NODE_TOOLS_H__

#define NODE_TOOLS_H__

#include <stdio.h>

#define READ_OK 0
#define READ_ERR_NO_DATA 1
#define READ_ERR_INVALID_ALLOC 2
#define READ_ERR_INVALID_FILE 3

typedef enum {NONE, RED, GREEN, BLUE} color_t;

typedef struct node node_t;

struct node
{
    char *data;
    color_t color;

    node_t *left;
    node_t *right;
};

node_t *node_free(node_t *node);
node_t *node_add(node_t *node, node_t *elem);
void node_delete(node_t **node, char *data);
node_t *node_search(node_t *head, char *data);
void node_output_pre_order(node_t *node, FILE *f);
void node_output_in_order(node_t *node, FILE *f);
void node_output_post_order(node_t *node, FILE *f);
void node_export_to_dot_eli(FILE *f, const char *node_data, node_t *node);

int node_read_by_file(char *filename, node_t **root);

#endif
