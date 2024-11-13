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

double max(double d1, double d2)
{
    if (d1 - d2 >= -EPS)
        return d1;
    else
        return d2;
}

double min(double d1, double d2)
{
    if (d2 - d1 >= -EPS)
        return d1;
    else
        return d2;
}

size_t static_push_by_delta(static_queue_t *queue, double *T, double delta, int T_min, int T_max, int T_prcs_min, int T_prcs_max, double *in_time, double *total_time, bool is_end)
{
    size_t total_pushes = 0;
    size_t additional_size_livetime = 0;
    size_t start_size = static_size(queue);


    while (delta - *T > -EPS)
    {
        if (static_push(rand_get(T_prcs_min, T_prcs_max), queue))
            return total_pushes;

        *in_time += *T;

        *total_time += *T * (start_size + additional_size_livetime);

        delta -= *T;
        *T = rand_get(T_min, T_max);

        additional_size_livetime++;
        total_pushes++;
    }

    *T -= delta;
    *total_time += delta * (start_size + additional_size_livetime);

    if (! is_end)
        *in_time += delta;

    return total_pushes;
}

double static_service(static_queue_t *fst_queue, static_queue_t *sec_queue)
{
    double tmp;

    bool flag = true;
    bool OA_is_empty = true;

    size_t fst_prcs_count = 0;

    double total_time = 0.0;
    double delta = 0.0;

    double T1, T2;
    double OA_request_time = 0.0;

    // INFO INFO INFO INFO INFO INFO INFO INFO 

    double downtime;

    size_t pushed = 0;
    bool is_end = false;

    size_t requests = 0;
    size_t max_requests = 0;

    size_t fst_prcs_last = 0;

    // ---

    double fst_sum_request_livetime = 0.0;
    double sec_sum_request_livetime = 0.0;

    size_t fst_total_push = 0;
    size_t fst_total_pop = 0;
    size_t sec_total_push = 0;
    size_t sec_total_pop = 0;

    size_t fst_hundred_push = 0;
    size_t fst_hundred_pop = 0;
    size_t sec_hundred_push = 0;
    size_t sec_hundred_pop = 0;

    // ---

    double model_time = 0;

    double fst_in_model_time = 0.0;
    double sec_in_model_time = 0.0;

    double fst_out_model_time = 0.0;
    double sec_out_model_time = 0.0;

    double fst_model_time = 0.0;
    double sec_model_time = 0.0;

    //

    size_t total_iterations = 0;

    // INFO INFO INFO INFO INFO INFO INFO INFO 

    srand(time(NULL));

    T1 = rand_get(T1_MIN, T1_MAX);
    T2 = rand_get(T2_MIN, T2_MAX);

    while (flag)
    {
        total_time += delta; // Основное время изменилось на delta

        if (fst_prcs_count == MAX_PRCS_COUNT + 1)
            is_end = true;

        pushed = static_push_by_delta(fst_queue, &T1, delta, T1_MIN, T1_MAX, T3_MIN, T3_MAX, &fst_in_model_time, &fst_sum_request_livetime, is_end);
        fst_hundred_push += pushed;
        requests += pushed;

        pushed = static_push_by_delta(sec_queue, &T2, delta, T2_MIN, T2_MAX, T4_MIN, T4_MAX, &sec_in_model_time, &sec_sum_request_livetime, is_end);
        sec_hundred_push += pushed;
        requests += pushed;

        if (requests > max_requests)
            max_requests = requests;

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
            printf("Average request lifetime: %.6lf units\n", fst_sum_request_livetime / fst_hundred_push);
            printf("pushed: %zu elements\n", fst_hundred_push);
            printf("popped: %zu elements\n", fst_hundred_pop);

            printf("\n2-nd type request:\n");
            printf("Elements count: %zu elements\n", static_size(sec_queue));
            printf("Average request lifetime: %.6lf units\n", sec_sum_request_livetime / sec_hundred_push);
            printf("pushed: %zu elements\n", sec_hundred_push);
            printf("popped: %zu elements\n", sec_hundred_pop);

            printf("\nGeneral stats\n");
            printf("Total pushed (per 100 requests): %zu elements\n", fst_hundred_push + sec_hundred_push);
            printf("Total popped (per 100 requests): %zu elements\n", fst_hundred_pop + sec_hundred_pop);
            printf("Time: %.6lf units\n", total_time);
            
            printf("\n--------------------------------");

            fst_total_push += fst_hundred_push;
            fst_total_pop += fst_hundred_pop;
            sec_total_push += sec_hundred_push;
            sec_total_pop += sec_hundred_pop;

            fst_hundred_push = 0;
            fst_hundred_pop = 0;
            sec_hundred_push = 0;
            sec_hundred_pop = 0;

            fst_sum_request_livetime = 0.0;
            sec_sum_request_livetime = 0.0;

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
                sec_hundred_pop++;
                sec_out_model_time += OA_request_time;

                OA_is_empty = false;
            }
        }
        else
        {
            fst_prcs_count++;

            fst_hundred_pop++;
            fst_out_model_time += OA_request_time;

            OA_is_empty = false;
        }

        if (OA_is_empty) // если очередь пустая, а элементов нет, переходим на delta_time = min(2-х заявок) + это время простоя
            delta = min(T1, T2);
        else
        {
            delta = OA_request_time;

            model_time += OA_request_time;
            requests--;
        }

        total_iterations++;
    }

    printf("\nTOTAL DATA\n\n");
    printf("Required size: %zu bytes\n", max_requests * sizeof(list_queue_t));
    printf("Total time: %.6lf units\n", total_time);

    printf("\nTotal pushed (1-st queue): %zu elements\n", fst_total_push);
    printf("Total popped (1-st queue): %zu elements\n", fst_total_pop);

    fst_model_time = max(fst_in_model_time, fst_out_model_time);
    printf("model time (1-st queue): %.6lf units\n", fst_model_time);

    tmp = fst_in_model_time / (T1_AVG);
    // printf("Calculated average elements count: %.6lf\n", tmp);
    printf("Error (in): %.6lf%% \n", 100.0 * fabs(tmp - fst_total_push) / min(tmp, fst_total_push));

    downtime = fst_in_model_time - fst_out_model_time;
    fst_model_time = downtime + fst_total_pop * (T3_AVG);
    printf("Error (out): %.6lf%% \n", 100.0 * fabs(fst_model_time - fst_in_model_time) / min(fst_model_time, fst_in_model_time));

    // --------------------------

    printf("\nTotal pushed (2-st queue): %zu elements\n", sec_total_push);
    printf("Total popped (2-st queue): %zu elements\n", sec_total_pop);

    sec_model_time = max(sec_in_model_time, sec_out_model_time);
    printf("model time (2-st queue): %.6lf units\n", sec_model_time);
    downtime = sec_in_model_time - sec_out_model_time;
    sec_model_time = downtime + sec_total_pop * (T4_AVG);
    printf("Error (out): %.6lf%% \n", 100.0 * fabs(sec_model_time - sec_in_model_time) / min(sec_model_time, sec_in_model_time));

    // printf("\nModel time: %0.6lf units\n", model_time); // НО ТУТ ХЗ

    printf("\n--------------------------------");

    return total_time;
}

void static_push_by_delta_no_stats(static_queue_t *queue, double *T, double delta, int T_min, int T_max, int T_prcs_min, int T_prcs_max)
{
    while (delta - *T > -EPS)
    {
        if (static_push(rand_get(T_prcs_min, T_prcs_max), queue))
            return;

        delta -= *T;
        *T = rand_get(T_min, T_max);
    }

    *T -= delta;
}

void static_service_no_stats(static_queue_t *fst_queue, static_queue_t *sec_queue)
{
    bool flag = true;
    bool OA_is_empty = true;

    size_t fst_prcs_count = 0;

    double delta = 0.0;

    double T1, T2;
    double OA_request_time = 0.0;

    srand(time(NULL));

    T1 = rand_get(T1_MIN, T1_MAX);
    T2 = rand_get(T2_MIN, T2_MAX);

    while (flag)
    {
        static_push_by_delta_no_stats(fst_queue, &T1, delta, T1_MIN, T1_MAX, T3_MIN, T3_MAX);

        static_push_by_delta_no_stats(sec_queue, &T2, delta, T2_MIN, T2_MAX, T4_MIN, T4_MAX);

        if (fst_prcs_count == MAX_PRCS_COUNT + 1)
        {
            flag = false;
            
            break;
        }

        if (static_pop(&OA_request_time, fst_queue))
        {
            if (static_pop(&OA_request_time, sec_queue))
            {
                OA_is_empty = true;
            }
            else
            {
                OA_is_empty = false;
            }
        }
        else
        {
            fst_prcs_count++;

            OA_is_empty = false;
        }

        if (OA_is_empty)
            delta = min(T1, T2);
        else
        {
            delta = OA_request_time;
        }
    }
}


// -----------------------------------------------






size_t list_push_by_delta_with_addresses(list_queue_t *queue, double *T, double delta, int T_min, int T_max, int T_prcs_min, int T_prcs_max, double *in_time, double *total_time, bool is_end)
{
    size_t total_pushes = 0;
    size_t additional_size_livetime = 0;
    size_t start_size = list_size(queue);


    while (delta - *T > -EPS)
    {
        list_push_with_addresses(rand_get(T_prcs_min, T_prcs_max), queue);
        *in_time += *T;

        *total_time += *T * (start_size + additional_size_livetime);

        delta -= *T;
        *T = rand_get(T_min, T_max);

        additional_size_livetime++;
        total_pushes++;
    }

    *T -= delta;
    *total_time += delta * (start_size + additional_size_livetime);

    if (! is_end)
        *in_time += delta;

    return total_pushes;
}

double list_service(list_queue_t *fst_queue, list_queue_t *sec_queue)
{
    double tmp;

    bool flag = true;
    bool OA_is_empty = true;

    size_t fst_prcs_count = 0;

    double total_time = 0.0;
    double delta = 0.0;

    double T1, T2;
    double OA_request_time = 0.0;

    // INFO INFO INFO INFO INFO INFO INFO INFO 

    double downtime;

    size_t pushed = 0;
    bool is_end = false;

    size_t requests = 0;
    size_t max_requests = 0;

    size_t fst_prcs_last = 0;

    // ---

    double fst_sum_request_livetime = 0.0;
    double sec_sum_request_livetime = 0.0;

    size_t fst_total_push = 0;
    size_t fst_total_pop = 0;
    size_t sec_total_push = 0;
    size_t sec_total_pop = 0;

    size_t fst_hundred_push = 0;
    size_t fst_hundred_pop = 0;
    size_t sec_hundred_push = 0;
    size_t sec_hundred_pop = 0;

    // ---

    double model_time = 0;

    double fst_in_model_time = 0.0;
    double sec_in_model_time = 0.0;

    double fst_out_model_time = 0.0;
    double sec_out_model_time = 0.0;

    double fst_model_time = 0.0;
    double sec_model_time = 0.0;

    //

    size_t total_iterations = 0;

    // INFO INFO INFO INFO INFO INFO INFO INFO 

    srand(time(NULL));

    T1 = rand_get(T1_MIN, T1_MAX);
    T2 = rand_get(T2_MIN, T2_MAX);

    while (flag)
    {
        total_time += delta; // Основное время изменилось на delta

        if (fst_prcs_count == MAX_PRCS_COUNT + 1)
            is_end = true;

        pushed = list_push_by_delta_with_addresses(fst_queue, &T1, delta, T1_MIN, T1_MAX, T3_MIN, T3_MAX, &fst_in_model_time, &fst_sum_request_livetime, is_end);
        fst_hundred_push += pushed;
        requests += pushed;

        pushed = list_push_by_delta_with_addresses(sec_queue, &T2, delta, T2_MIN, T2_MAX, T4_MIN, T4_MAX, &sec_in_model_time, &sec_sum_request_livetime, is_end);
        sec_hundred_push += pushed;
        requests += pushed;

        if (requests > max_requests)
            max_requests = requests;

        if (fst_prcs_count == MAX_PRCS_COUNT + 1)
        {
            flag = false;
            
            break;
        }
        else if (fst_prcs_count % INFO_PRCS_COUNT == 0 && fst_prcs_count && fst_prcs_count != fst_prcs_last)
        {
            printf("\n1-st type request:\n");
            printf("processed: %zu elements\n", fst_prcs_count);
            printf("Elements count: %zu elements\n", list_size(fst_queue));
            printf("Average request lifetime: %.6lf units\n", fst_sum_request_livetime / fst_hundred_push);
            printf("pushed: %zu elements\n", fst_hundred_push);
            printf("popped: %zu elements\n", fst_hundred_pop);

            printf("\n2-nd type request:\n");
            printf("Elements count: %zu elements\n", list_size(sec_queue));
            printf("Average request lifetime: %.6lf units\n", sec_sum_request_livetime / sec_hundred_push);
            printf("pushed: %zu elements\n", sec_hundred_push);
            printf("popped: %zu elements\n", sec_hundred_pop);

            printf("\nGeneral stats\n");
            printf("Total pushed (per 100 requests): %zu elements\n", fst_hundred_push + sec_hundred_push);
            printf("Total popped (per 100 requests): %zu elements\n", fst_hundred_pop + sec_hundred_pop);
            printf("Time: %.6lf units\n", total_time);
            
            printf("\n--------------------------------");

            fst_total_push += fst_hundred_push;
            fst_total_pop += fst_hundred_pop;
            sec_total_push += sec_hundred_push;
            sec_total_pop += sec_hundred_pop;

            fst_hundred_push = 0;
            fst_hundred_pop = 0;
            sec_hundred_push = 0;
            sec_hundred_pop = 0;

            fst_sum_request_livetime = 0.0;
            sec_sum_request_livetime = 0.0;

            fst_prcs_last = fst_prcs_count;
        }

        if (list_pop(&OA_request_time, fst_queue))
        {
            if (list_pop(&OA_request_time, sec_queue))
            {
                OA_is_empty = true;
                OA_request_time = 0.0;
            }
            else
            {
                sec_hundred_pop++;
                sec_out_model_time += OA_request_time;

                OA_is_empty = false;
            }
        }
        else
        {
            fst_prcs_count++;

            fst_hundred_pop++;
            fst_out_model_time += OA_request_time;

            OA_is_empty = false;
        }

        if (OA_is_empty) // если очередь пустая, а элементов нет, переходим на delta_time = min(2-х заявок) + это время простоя
            delta = min(T1, T2);
        else
        {
            delta = OA_request_time;

            model_time += OA_request_time;
            requests--;
        }

        total_iterations++;
    }

    printf("\nTOTAL DATA\n\n");
    printf("Required size: %zu bytes\n", max_requests * sizeof(list_queue_t));
    printf("Total time: %.6lf units\n", total_time);

    printf("\nTotal pushed (1-st queue): %zu elements\n", fst_total_push);
    printf("Total popped (1-st queue): %zu elements\n", fst_total_pop);

    fst_model_time = max(fst_in_model_time, fst_out_model_time);
    printf("model time (1-st queue): %.6lf units\n", fst_model_time);

    tmp = fst_in_model_time / (T1_AVG);
    // printf("Calculated average elements count: %.6lf\n", tmp);
    printf("Error (in): %.6lf%% \n", 100.0 * fabs(tmp - fst_total_push) / min(tmp, fst_total_push));

    downtime = fst_in_model_time - fst_out_model_time;
    fst_model_time = downtime + fst_total_pop * (T3_AVG);
    printf("Error (out): %.6lf%% \n", 100.0 * fabs(fst_model_time - fst_in_model_time) / min(fst_model_time, fst_in_model_time));

    // --------------------------

    printf("\nTotal pushed (2-st queue): %zu elements\n", sec_total_push);
    printf("Total popped (2-st queue): %zu elements\n", sec_total_pop);

    sec_model_time = max(sec_in_model_time, sec_out_model_time);
    printf("model time (2-st queue): %.6lf units\n", sec_model_time);
    downtime = sec_in_model_time - sec_out_model_time;
    sec_model_time = downtime + sec_total_pop * (T4_AVG);
    printf("Error (out): %.6lf%% \n", 100.0 * fabs(sec_model_time - sec_in_model_time) / min(sec_model_time, sec_in_model_time));

    // printf("\nModel time: %0.6lf units\n", model_time); // НО ТУТ ХЗ

    printf("\n--------------------------------");

    return total_time;
}

void list_push_by_delta_no_stats(list_queue_t *queue, double *T, double delta, int T_min, int T_max, int T_prcs_min, int T_prcs_max)
{
    while (delta - *T > -EPS)
    {
        list_push(rand_get(T_prcs_min, T_prcs_max), queue);

        delta -= *T;
        *T = rand_get(T_min, T_max);
    }

    *T -= delta;
}

void list_service_no_stats(list_queue_t *fst_queue, list_queue_t *sec_queue)
{
    bool flag = true;
    bool OA_is_empty = true;

    size_t fst_prcs_count = 0;

    double delta = 0.0;

    double T1, T2;
    double OA_request_time = 0.0;

    srand(time(NULL));

    T1 = rand_get(T1_MIN, T1_MAX);
    T2 = rand_get(T2_MIN, T2_MAX);

    while (flag)
    {
        list_push_by_delta_no_stats(fst_queue, &T1, delta, T1_MIN, T1_MAX, T3_MIN, T3_MAX);

        list_push_by_delta_no_stats(sec_queue, &T2, delta, T2_MIN, T2_MAX, T4_MIN, T4_MAX);

        if (fst_prcs_count == MAX_PRCS_COUNT + 1)
        {
            flag = false;
            
            break;
        }

        if (list_pop(&OA_request_time, fst_queue))
        {
            if (list_pop(&OA_request_time, sec_queue))
            {
                OA_is_empty = true;
            }
            else
            {
                OA_is_empty = false;
            }
        }
        else
        {
            fst_prcs_count++;

            OA_is_empty = false;
        }

        if (OA_is_empty)
            delta = min(T1, T2);
        else
        {
            delta = OA_request_time;
        }
    }
}

void time_comparing(static_queue_t *static_fst_queue, static_queue_t *static_sec_queue, list_queue_t *list_fst_queue, list_queue_t *list_sec_queue)
{
    long time_static = 0;
    long time_list = 0;

    struct timespec t_beg, t_end;

    printf("TIMES (nsec)\n");

    for (size_t i = 0; i < TIME_COMPARING_ITER; i++)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        static_service_no_stats(static_fst_queue, static_sec_queue);
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

        static_free(static_fst_queue);
        static_free(static_sec_queue);

        time_static += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
    }

    printf("Average static worktime: %.6lf\n", (double)time_static / TIME_COMPARING_ITER);

    for (size_t i = 0; i < TIME_COMPARING_ITER; i++)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        list_service_no_stats(list_fst_queue, list_sec_queue);
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

        list_free(list_fst_queue);
        list_free(list_sec_queue);

        time_list += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
    }

    printf("Average list worktime: %.6lf\n", (double)time_list / TIME_COMPARING_ITER);
}
