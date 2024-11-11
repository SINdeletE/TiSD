#include "static_queue_tools.h"

int static_empty(static_queue_t *queue)
{
    if (queue->size == 0)
        return 1;
    
    return 0;
}

size_t static_size(static_queue_t *queue)
{
    return queue->size;
}

int static_push(double time, static_queue_t *queue)
{
    if (static_size(queue) == MAX_QUEUE_SIZE)
        return 1;

    *queue->tail = time;

    if (queue->tail == queue->times + MAX_QUEUE_SIZE - 1)
        queue->tail = queue->times;
    else
        queue->tail++;

    queue->size++;

    return 0;
}

int static_pop(double *time, static_queue_t *queue)
{
    if (static_empty(queue))
        return 1;
    
    *time = *queue->head;
    *queue->head = 0;

    if (queue->head == queue->times + MAX_QUEUE_SIZE - 1)
        queue->head = queue->times;
    else
        queue->head++;

    queue->size--;

    return 0;
}
