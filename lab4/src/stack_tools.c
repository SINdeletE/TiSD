#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack_tools.h"

size_t static_stack_size(static_stack_t *stack)
{
    return stack->size;
}

void static_stack_free(static_stack_t *stack)
{
    char c;
    
    while (! static_stack_pop(&c, stack));
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
    stack->stack[static_stack_size(stack) - 1] = 0;

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
                return CHECK_ERR_STACK_INCORRECT_PUSH;
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
    if (static_stack_size(stack) != 0)
    {
        for (size_t i = static_stack_size(stack) - 1; i > 0; i--)
            printf("%c\n", stack->stack[i]);

        printf("%c\n", stack->stack[0]);
    }
}

// __________________________________________________________________________________

void list_stack_free(list_stack_t **head)
{
    char c;

    while (! list_stack_pop(&c, head));
}

int list_stack_pop(char *c, list_stack_t **head)
{
    list_stack_t *tmp = NULL;

    if (! *head)
        return 1;

    tmp = *head;
    *c = tmp->value;

    *head = (*head)->next;

    free(tmp);

    return 0;
}

int list_stack_push(char c, list_stack_t **head)
{
    list_stack_t *tmp;

    if ((tmp = malloc(sizeof(list_stack_t))) == NULL)
        return 1;

    tmp->value = c;
    tmp->next = *head;

    *head = tmp;

    return 0;
}

int list_stack_string_check(char *str, list_stack_t **head)
{
    char c;

    for (char *p = str; *p != '\0'; p++)
        if (strchr(BRACKETS_OPEN, *p) != NULL)
        {
            if (list_stack_push(*p, head))
                return CHECK_ERR_STACK_INCORRECT_PUSH;
        }
        else if (strchr(BRACKETS_CLOSE, *p) != NULL)
        {
            if (list_stack_pop(&c, head))
                return CHECK_INCORRECT;

            if (! brackets_with_equal_type(c, *p))
                return CHECK_INCORRECT;
        }
    
    if (list_stack_pop(&c, head))
        return CHECK_CORRECT;

    list_stack_push(c, head);

    return CHECK_INCORRECT;
}

void list_stack_show(list_stack_t *head)
{
    list_stack_t *cur = head;

    for (; cur; cur = cur->next)
        printf("%c\n", cur->value);
}
