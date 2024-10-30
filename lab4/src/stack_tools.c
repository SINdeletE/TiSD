#include <stdio.h>
#include <string.h>

#include "stack_tools.h"

size_t static_stack_size(static_stack_t *stack)
{
    return stack->size;
}

void static_stack_free(static_stack_t *stack)
{
    for (size_t i = 0; i < static_stack_size(stack); i++)
        stack->stack[i] = 0;
    
    stack->size = 0;
}

int brackets_with_equal_type(char open, char close)
{
    size_t len = strlen(ALL_BRACKETS) / 2;

    for (size_t i = 0; i < len; i++)
        if (open == ALL_BRACKETS[i] && close == ALL_BRACKETS[i + len])
            return 1;
    
    return 0;
}

int static_stack_pop(char *data, static_stack_t *stack)
{
    if (! static_stack_size(stack))
        return 1;
    
    *data = stack->stack[static_stack_size(stack) - 1];
    stack->size--;

    return 0;
}

int static_stack_push(char data, static_stack_t *stack)
{
    if (static_stack_size(stack) >= STACK_MAX_SIZE)
        return 1;

    stack->stack[static_stack_size(stack)] = data;
    stack->size++;

    return 0;
}

int static_stack_string_check(char *str, static_stack_t *stack)
{
    char c;

    for (char *p = str; *p != '\0'; p++)
        if (strchr(BRACKETS_OPEN, *p) != NULL)
        {
            if (static_stack_push(*p, stack))
                return CHECK_ERR_STACK_OVERFLOW;
        }
        else if (strchr(BRACKETS_CLOSE, *p) != NULL)
        {
            if (static_stack_pop(&c, stack))
                return CHECK_INCORRECT;

            if (! brackets_with_equal_type(c, *p))
                return CHECK_INCORRECT;
        }
    
    if (static_stack_pop(&c, stack))
        return CHECK_CORRECT;

    static_stack_push(c, stack);

    return CHECK_INCORRECT;
}

void static_stack_show(static_stack_t *stack)
{
    for (size_t i = 0; i < static_stack_size(stack); i++)
        printf("%c\n", stack->stack[i]);
}

// __________________________________________________________________________________

// void list_stack_free(list_stack_t *head)
// {
//     list_stack_t *cur = head;

//     for (; cur; cur->next)
//         stack->stack[i] = 0;
    
//     stack->size = 0;
// }
