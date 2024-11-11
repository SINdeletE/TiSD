#include <stdlib.h>

#include "list_queue_tools.h"

int list_empty(list_queue_t *queue)
{
    if (queue->head == NULL)
        return 1;
    
    return 0;
}

size_t list_size(list_queue_t *queue)
{
    size_t size;

    for (queue_data_t *iter = queue->head; iter->next; iter = iter->next)
        size++;

    return size;
}

int list_push(double time, list_queue_t *queue)
{
    queue_data_t *tmp = NULL;

    if ((tmp = calloc(1, sizeof(queue_data_t))) == NULL)
        return 1;

    tmp->time = time;
    tmp->next = NULL;

    if (! queue->head)
        queue->head = tmp;
    
    if (! queue->tail)
        queue->tail = tmp;
    else
    {
        queue->tail->next = tmp;
        queue->tail = queue->tail->next;
    }

    return 0;
}

int list_pop(double *time, list_queue_t *queue)
{
    queue_data_t *tmp = NULL;
    
    if (list_empty(queue))
        return 1;

    *time = queue->head->time;

    tmp = queue->head;
    queue->head = queue->head->next;

    if (queue->head == NULL)
        queue->tail = NULL;

    free(tmp);

    return 0;
}
