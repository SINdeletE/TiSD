#ifndef STATIC_QUEUE_TOOLS_H__

#define STATIC_QUEUE_TOOLS_H__

#include <stddef.h>

#define MAX_QUEUE_SIZE 256

typedef struct
{
    double times[MAX_QUEUE_SIZE];
    size_t size;

    double *head;
    double *tail;
} static_queue_t;

void static_free(static_queue_t *queue);
void static_init(static_queue_t *queue);
int static_empty(static_queue_t *queue);
size_t static_size(static_queue_t *queue);
int static_push(double time, static_queue_t *queue);
int static_pop(double *time, static_queue_t *queue);

#endif
