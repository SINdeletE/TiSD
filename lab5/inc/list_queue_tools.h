#ifndef LIST_QUEUE_TOOLS_H__

#define LIST_QUEUE_TOOLS_H__

typedef struct queue_data queue_data_t;

struct queue_data
{
    double time;

    queue_data_t *next;
};

typedef struct
{
    queue_data_t *head;
    queue_data_t *tail;
} list_queue_t;

int list_empty(list_queue_t *queue);
size_t list_size(list_queue_t *queue);
int list_push(double time, list_queue_t *queue);
int list_pop(double *time, list_queue_t *queue);

#endif
