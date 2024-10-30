#ifndef STACK_TOOLS_H__

#define STACK_TOOLS_H__

#include <stddef.h>

#define STACK_MAX_SIZE 32

#define CHECK_CORRECT 0
#define CHECK_INCORRECT 1
#define CHECK_ERR_STACK_OVERFLOW 2

#define BRACKETS_OPEN "({["
#define BRACKETS_CLOSE ")}]"

#define ALL_BRACKETS "({[)}]"

typedef struct
{
    char stack[STACK_MAX_SIZE];
    size_t size;
} static_stack_t;


// typedef struct list_stack_t list_stack_t;

// typedef struct list_stack_t
// {
//     char value;

//     list_stack_t *next;
// };

void static_stack_free(static_stack_t *stack);
int static_stack_string_check(char *str, static_stack_t *stack);
void static_stack_show(static_stack_t *stack);
int static_stack_pop(char *data, static_stack_t *stack);
int static_stack_push(char data, static_stack_t *stack);

#endif
