#ifndef STATIC_QUEUE_TOOLS_H__

#define STATIC_QUEUE_TOOLS_H__

#include <stddef.h>

#define MAX_QUEUE_SIZE 512

typedef struct
{
    double times[MAX_QUEUE_SIZE];
    size_t size;

    double *head;
    double *tail;
} static_queue_t;

int static_empty(static_queue_t *queue);
size_t static_size(static_queue_t *queue);
int static_push(double time, static_queue_t *queue);
int static_pop(double *time, static_queue_t *queue);

#endif
