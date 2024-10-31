#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stack_tools.h"
#include "addresses_tools.h"

#define STR_TABLE_SIZE 8

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

    addresses_add(tmp);

    return 0;
}

int list_stack_pop_without_addresses(char *c, list_stack_t **head)
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
        printf("%c \t %p\n", cur->value, (void *)cur);
}

void stack_statistics(static_stack_t *static_stack, list_stack_t **list_stack_head)
{
    char c;

    struct timespec t_beg, t_end;

    size_t ss_mem_size = 0, ls_mem_size = 0;
    long ss_time_push = 0, ls_time_push = 0;
    long ss_time_pop = 0, ls_time_pop = 0;

    long ss_time_push_sum = 0, ls_time_push_sum = 0;
    long ss_time_pop_sum = 0, ls_time_pop_sum = 0;
    size_t elems = 0;

    static_stack_free(static_stack);
    list_stack_free(list_stack_head);

    printf("\nSTATISTICS (time in nsec) (memory in bytes) (STATIC STACK SIZE IS %d)\n", STACK_MAX_SIZE);

    printf("N count |");
    printf("PUSH t (static) |");
    printf(" POP t (static) |");
    printf("  PUSH t (list) |");
    printf("   POP t (list) |");
    printf("Memory (static) |");
    printf(" Memory (list)  |");
    printf("  best t (PUSH) |");
    printf("  best t (POP)  |");
    printf("  best memory   |\n");

    for (size_t n_elems = 2; n_elems < STACK_MAX_SIZE; n_elems *= 2)
    {
        ss_time_push = 0;
        ls_time_push = 0;
        ss_time_pop = 0;
        ls_time_pop = 0;

        ss_mem_size = sizeof(static_stack_t);
        ls_mem_size = sizeof(list_stack_t) * n_elems + sizeof(list_stack_t *);

        // // Чтобы адреса не портили результаты статистики
        // while (addresses_cap() < addresses_size() + n_elems)
        //     addresses_realloc();

        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            for (size_t i = 0; i < n_elems; i++)
                static_stack_push('[', static_stack);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
            ss_time_push += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            

            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            for (size_t i = 0; i < n_elems; i++)
                static_stack_pop(&c, static_stack);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
            ss_time_pop += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            
        }

        ss_time_push /= ITER_COUNT;
        ss_time_pop /= ITER_COUNT;

        for (size_t i = 0; i < ITER_COUNT; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            for (size_t i = 0; i < n_elems; i++)
                list_stack_push('[', list_stack_head);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
            ls_time_push += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            

            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            for (size_t i = 0; i < n_elems; i++)
                list_stack_pop_without_addresses(&c, list_stack_head);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
            ls_time_pop += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            
        }

        ls_time_push /= ITER_COUNT;
        ls_time_pop /= ITER_COUNT;

        printf("%-*zu|", STR_TABLE_SIZE, n_elems);
        printf("%-*ld|", STR_TABLE_SIZE * 2, ss_time_push);
        printf("%-*ld|", STR_TABLE_SIZE * 2, ss_time_pop);
        printf("%-*ld|", STR_TABLE_SIZE * 2, ls_time_push);
        printf("%-*ld|", STR_TABLE_SIZE * 2, ls_time_pop);
        printf("%-*zu|", STR_TABLE_SIZE * 2, ss_mem_size);
        printf("%-*zu|", STR_TABLE_SIZE * 2, ls_mem_size);
        if (ss_time_push < ls_time_push)
            printf("     STATIC     |");
        else if (ss_time_push > ls_time_push)
            printf("      LIST      |");
        else 
            printf("      EQUAL     |");

        if (ss_time_pop < ls_time_pop)
            printf("     STATIC     |");
        else if (ss_time_pop > ls_time_pop)
            printf("      LIST      |");
        else 
            printf("      EQUAL     |");
        
        if (ss_mem_size < ls_mem_size)
            printf("     STATIC     |");
        else if (ss_mem_size > ls_mem_size)
            printf("      LIST      |");
        else 
            printf("      EQUAL     |");
        
        printf("\n");

        ss_time_push_sum += ss_time_push;
        ss_time_pop_sum += ss_time_pop;

        ls_time_push_sum += ls_time_push;
        ls_time_pop_sum += ls_time_pop;

        elems += n_elems;
    }

    printf("\nAverage push time (static): %ld\n", ss_time_push_sum / elems);
    printf("\nAverage pop time (static): %ld\n", ss_time_pop_sum / elems);
    printf("\nAverage push time (list): %ld\n", ls_time_push_sum / elems);
    printf("\nAverage pop time (list): %ld\n", ls_time_pop_sum / elems);

    static_stack_free(static_stack);
    list_stack_free(list_stack_head);
}
