#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#include "service.h"

#define EPS 1e-8

double rand_get(double l, double r)
{
    double random_num = (double)rand() / RAND_MAX;

    return (random_num * (r - l)) + l;
}

double min(double d1, double d2)
{
    if (d1 - d2 >= -EPS)
        return d1;
    else
        return d2;
}

size_t static_push_by_delta(static_queue_t *queue, double *T, double delta, int T_min, int T_max, int T_prcs_min, int T_prcs_max)
{
    size_t total_pushes = 0;

    while (delta - *T > -EPS)
    {
        static_push(rand_get(T_prcs_min, T_prcs_max), queue);

        delta -= *T;
        *T = rand_get(T_min, T_max);

        total_pushes++;
    }

    *T -= delta;

    return total_pushes;
}

void static_service(static_queue_t *fst_queue, static_queue_t *sec_queue)
{
    bool flag = true;
    bool OA_is_empty = true;

    size_t fst_prcs_count = 0;

    double total_time = 0.0;
    double delta = 0.0;

    double T1, T2;
    double OA_request_time = 0.0;

    // INFO INFO INFO INFO INFO INFO INFO INFO 

    size_t fst_prcs_last = 0;

    size_t fst_total_push = 0;
    size_t fst_total_pop = 0;

    size_t sec_total_push = 0;
    size_t sec_total_pop = 0;

    size_t total_iterations = 0;

    // INFO INFO INFO INFO INFO INFO INFO INFO 

    srand(time(NULL));

    T1 = rand_get(T1_MIN, T1_MAX);
    T2 = rand_get(T2_MIN, T2_MAX);

    while (flag)
    {
        total_time += delta; // Основное время изменилось на delta

        fst_total_push += static_push_by_delta(fst_queue, &T1, delta, T1_MIN, T1_MAX, T3_MIN, T3_MAX);
        sec_total_push += static_push_by_delta(sec_queue, &T2, delta, T2_MIN, T2_MAX, T4_MIN, T4_MAX);

        if (fst_prcs_count == MAX_PRCS_COUNT + 1)
        {
            flag = false;
            
            break;
        }
        else if (fst_prcs_count % INFO_PRCS_COUNT == 0 && fst_prcs_count && fst_prcs_count != fst_prcs_last)
        {
            printf("\n1-st type request:\n");
            printf("processed: %zu elements\n", fst_prcs_count);
            printf("Elements count: %zu elements\n", static_size(fst_queue));
            printf("total pushed: %zu elements\n", fst_total_push);
            printf("total popped: %zu elements\n", fst_total_pop);

            printf("\n2-nd type request:\n");
            printf("Elements count: %zu elements\n", static_size(sec_queue));
            printf("total pushed: %zu elements\n", sec_total_push);
            printf("total popped: %zu elements\n", sec_total_pop);

            printf("\nTotal pushed: %zu elements\n", fst_total_push + sec_total_push);
            printf("Total popped: %zu elements\n", fst_total_pop + sec_total_pop);
            printf("Time: %.6lf units\n", total_time);
            
            printf("\n--------------------------------");

            fst_prcs_last = fst_prcs_count;
        }

        if (static_pop(&OA_request_time, fst_queue))
        {
            if (static_pop(&OA_request_time, sec_queue))
            {
                OA_is_empty = true;
                OA_request_time = 0.0;
            }
            else
            {
                sec_total_pop++;

                OA_is_empty = false;
            }
        }
        else
        {
            fst_prcs_count++;

            fst_total_pop++;

            OA_is_empty = false;
        }

        if (OA_is_empty) // если очередь пустая, а элементов нет, переходим на delta_time = min(2-х заявок)
            delta = min(T1, T2);
        else
            delta = OA_request_time;

        total_iterations++;
    }

    printf("\nTOTAL DATA\n\n");
    printf("required size: %zu bytes\n", sizeof(static_queue_t) + sizeof(static_queue_t));
    printf("model time: %.6lf units\n", total_time);
    printf("total iterations: %zu\n", total_iterations);

    printf("\n--------------------------------");
}
