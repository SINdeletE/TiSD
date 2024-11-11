#include <stdlib.h>

#include "service.h"

double rand_get(double l, double r)
{
    double random_num = rand() / RAND_MAX;

    return (random_num * (r - l)) + l;
}

int static_service(static_queue_t *fst_queue, static_queue_t *sec_queue)
{
    int flag = 1;
    size_t fst_prcs_count = 0;

    double T1, T2;

    double request_time;
    double OA_prcs_time = 0.0;

    T1 = rand_get(T1_MIN, T1_MAX);
    T2 = rand_get(T2_MIN, T2_MAX);

    while (flag)
    {
        if (static_pop(&request_time, fst_queue))
        {
            if (static_pop(&request_time, sec_queue))
                request_time = 0.0;
        }
        else
            fst_prcs_count++;

        OA_prcs_time = request_time;
    }
}
