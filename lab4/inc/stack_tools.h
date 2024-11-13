#ifndef STACK_TOOLS_H__

#define STACK_TOOLS_H__

#include <stddef.h>

#define STACK_MAX_SIZE 2049
#define ITER_COUNT 1000

#define CHECK_CORRECT 0
#define CHECK_INCORRECT 1
#define CHECK_ERR_STACK_INCORRECT_PUSH 2

#define BRACKETS_OPEN "({["
#define BRACKETS_CLOSE ")}]"

#define ALL_BRACKETS "({[)}]"

typedef struct
{
    char stack[STACK_MAX_SIZE];
    size_t size;
} static_stack_t;

typedef struct list_stack_t_ list_stack_t;

typedef struct list_stack_t_
{
    char value;

    list_stack_t *next;
} a_t;

void static_stack_free(static_stack_t *stack);
int static_stack_string_check(char *str, static_stack_t *stack);
void static_stack_show(static_stack_t *stack);
int static_stack_pop(char *data, static_stack_t *stack);
int static_stack_push(char data, static_stack_t *stack);

void list_stack_free(list_stack_t **head);
int list_stack_string_check(char *str, list_stack_t **head);
void list_stack_show(list_stack_t *head);
int list_stack_pop(char *c, list_stack_t **head);
int list_stack_push(char c, list_stack_t **head);

void stack_statistics(static_stack_t *static_stack, list_stack_t **list_stack_head);

#endif
