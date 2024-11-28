#ifndef NODE_TOOLS_H__

#define NODE_TOOLS_H__

#include <stdio.h>

#define READ_OK 0
#define READ_ERR_NO_DATA 1
#define READ_ERR_INVALID_ALLOC 2
#define READ_ERR_INVALID_FILE 3

#define STAT_OK 0
#define STAT_ERR_INVALID_FILE 1
#define STAT_ERR_INVALID_READ 2

#define MAX_ELEMS_COUNT 1024
#define MAX_ITER_COUNT 1000

typedef struct node node_t;

struct node
{
    char *data;

    node_t *left;
    node_t *right;
    
    int height;
};

node_t *node_free(node_t *node);
node_t *node_alloc(char *data);
int node_height(node_t *node);
node_t *node_add(node_t *node, node_t *elem);
void node_delete(node_t **node, char *data);
node_t *node_search(node_t *node, char *data, int *compares);
void node_output_pre_order(node_t *node, FILE *f);
void node_output_in_order(node_t *node, FILE *f);
void node_output_post_order(node_t *node, FILE *f);
void node_export_to_dot_eli(FILE *f, const char *node_data, node_t *node);
node_t *node_cpy(node_t *node);

node_t *node_left_max(node_t *node);
node_t *node_right_min(node_t *node);
void node_data_swap(node_t *node_a, node_t *node_b);

// size_t node_count_and_color(node_t *head, char c);

// int node_height(node_t *node);
// node_t *node_random_tree(node_t **searched_element ,int *tree_h, int *tree_power);
// int node_statistics(char *filename, char c);

int node_read_by_file(char *filename, node_t **root);

#endif
