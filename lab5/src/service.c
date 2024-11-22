#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#include "service.h"

#define STR_TABLE_SIZE 8

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

size_t static_push_by_delta(static_queue_t *queue, double *T, double delta, const double T_min, const double T_max, const double T_prcs_min, const double T_prcs_max, double *in_time, double *total_time, bool is_end)
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

double static_service(static_queue_t *fst_queue, static_queue_t *sec_queue, double T1_MIN, double T1_MAX, double T2_MIN, double T2_MAX, double T3_MIN, double T3_MAX, double T4_MIN, double T4_MAX)
{
    double tmp;

    bool flag = true;
    bool OA_is_empty = true;

    const double T1_AVG = (T1_MAX + T1_MIN) / 2.0;
    const double T2_AVG = (T2_MAX + T2_MIN) / 2.0;
    const double T3_AVG = (T3_MAX + T3_MIN) / 2.0;
    const double T4_AVG = (T4_MAX + T4_MIN) / 2.0;

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
    double fst_total_lifetime = 0.0;
    double sec_sum_request_livetime = 0.0;
    double sec_total_lifetime = 0.0;

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

            fst_total_lifetime += fst_sum_request_livetime / fst_hundred_push;
            sec_total_lifetime += sec_sum_request_livetime / sec_hundred_push;

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
    printf("Required size: %zu bytes\n", sizeof(static_queue_t));
    printf("Total time: %.6lf units\n", total_time);

    printf("\nTotal pushed (1-st queue): %zu elements\n", fst_total_push);
    printf("Total popped (1-st queue): %zu elements\n", fst_total_pop);
    printf("Average lifetime %.6lf\n", fst_total_lifetime / 10);

    if (T1_AVG - T3_AVG > -EPS)
        fst_model_time = fst_in_model_time;
    else
        fst_model_time = fst_out_model_time;
    printf("model time (1-st queue): %.6lf units\n", fst_model_time);

    if (fst_in_model_time > fst_out_model_time)
        printf("Downtime: %.6lf\n", fst_in_model_time - fst_out_model_time);

    tmp = fst_in_model_time / (T1_AVG);
    // printf("Calculated average elements count: %.6lf\n", tmp);
    printf("Error (in): %.6lf%% \n", 100.0 * fabs(tmp - fst_total_push) / min(tmp, fst_total_push));

    downtime = fst_in_model_time - fst_out_model_time;
    fst_model_time = downtime + fst_total_pop * (T3_AVG);
    printf("Error (out): %.6lf%% \n", 100.0 * fabs(fst_model_time - fst_in_model_time) / min(fst_model_time, fst_in_model_time));
    
    if (T1_AVG - T3_AVG > -EPS)
    {
        tmp = fst_total_push * (T1_AVG);
        printf("Error: %.6lf%% \n", 100.0 * fabs(tmp - fst_model_time) / fst_model_time);
    }
    else
    {
        tmp = fst_total_pop * (T3_AVG);
        printf("Error: %.6lf%% \n", 100.0 * fabs(tmp - fst_model_time) / fst_model_time);
    }

    // --------------------------

    printf("\nTotal pushed (2-st queue): %zu elements\n", sec_total_push);
    printf("Total popped (2-st queue): %zu elements\n", sec_total_pop);
    printf("Average lifetime %.6lf\n", sec_total_lifetime / 10);

    if (T2_AVG - T4_AVG > -EPS)
        sec_model_time = sec_in_model_time;
    else
        sec_model_time = sec_out_model_time;
    printf("model time (2-st queue): %.6lf units\n", sec_model_time);
    if (sec_in_model_time > sec_out_model_time)
        printf("Downtime: %.6lf\n", sec_in_model_time - sec_out_model_time);

    tmp = sec_in_model_time / (T2_AVG);
    // printf("Calculated average elements count: %.6lf\n", tmp);
    printf("Error (in): %.6lf%% \n", 100.0 * fabs(tmp - sec_total_push) / min(tmp, sec_total_push));

    downtime = sec_in_model_time - sec_out_model_time;
    sec_model_time = downtime + sec_total_pop * (T4_AVG);
    printf("Error (out): %.6lf%% \n", 100.0 * fabs(sec_model_time - sec_in_model_time) / min(sec_model_time, sec_in_model_time));

    if (T2_AVG - T4_AVG > -EPS)
    {
        tmp = sec_total_push * (T2_AVG);
        printf("Error: %.6lf%% \n", 100.0 * fabs(tmp - sec_in_model_time) / sec_in_model_time);
    }
    else
    {
        tmp = sec_total_pop * (T4_AVG);
        printf("Error: %.6lf%% \n", 100.0 * fabs(tmp - sec_out_model_time) / sec_out_model_time);
    }

    // printf("\nModel time: %0.6lf units\n", model_time); // НО ТУТ ХЗ

    printf("\n--------------------------------");

    return total_time;
}

void static_push_by_delta_no_stats(static_queue_t *queue, double *T, double delta, const double T_min, const double T_max, const double T_prcs_min, const double T_prcs_max)
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

void static_service_no_stats(static_queue_t *fst_queue, static_queue_t *sec_queue, double T1_MIN, double T1_MAX, double T2_MIN, double T2_MAX, double T3_MIN, double T3_MAX, double T4_MIN, double T4_MAX)
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






size_t list_push_by_delta_with_addresses(list_queue_t *queue, double *T, double delta, const double T_min, const double T_max, const double T_prcs_min, const double T_prcs_max, double *in_time, double *total_time, bool is_end)
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

double list_service(list_queue_t *fst_queue, list_queue_t *sec_queue, double T1_MIN, double T1_MAX, double T2_MIN, double T2_MAX, double T3_MIN, double T3_MAX, double T4_MIN, double T4_MAX)
{
    double tmp;

    bool flag = true;
    bool OA_is_empty = true;

    const double T1_AVG = (T1_MAX + T1_MIN) / 2.0;
    const double T2_AVG = (T2_MAX + T2_MIN) / 2.0;
    const double T3_AVG = (T3_MAX + T3_MIN) / 2.0;
    const double T4_AVG = (T4_MAX + T4_MIN) / 2.0;

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

    double fst_total_lifetime = 0.0;
    double sec_total_lifetime = 0.0;

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

            fst_total_lifetime += fst_sum_request_livetime / fst_hundred_push;
            sec_total_lifetime += sec_sum_request_livetime / sec_hundred_push;

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
    printf("Required size: %zu bytes\n", max_requests * sizeof(queue_data_t) + sizeof(list_queue_t));
    printf("Total time: %.6lf units\n", total_time);

    printf("\nTotal pushed (1-st queue): %zu elements\n", fst_total_push);
    printf("Total popped (1-st queue): %zu elements\n", fst_total_pop);
    printf("Average lifetime %.6lf\n", fst_total_lifetime / 10);

    if (T1_AVG - T3_AVG > -EPS)
        fst_model_time = fst_in_model_time;
    else
        fst_model_time = fst_out_model_time;
    printf("model time (1-st queue): %.6lf units\n", fst_model_time);

    if (fst_in_model_time > fst_out_model_time)
        printf("Downtime: %.6lf\n", fst_in_model_time - fst_out_model_time);

    tmp = fst_in_model_time / (T1_AVG);
    // printf("Calculated average elements count: %.6lf\n", tmp);
    printf("Error (in): %.6lf%% \n", 100.0 * fabs(tmp - fst_total_push) / min(tmp, fst_total_push));

    downtime = fst_in_model_time - fst_out_model_time;
    fst_model_time = downtime + fst_total_pop * (T3_AVG);
    printf("Error (out): %.6lf%% \n", 100.0 * fabs(fst_model_time - fst_in_model_time) / min(fst_model_time, fst_in_model_time));
    
    if (T1_AVG - T3_AVG > -EPS)
    {
        tmp = fst_total_push * (T1_AVG);
        printf("Error: %.6lf%% \n", 100.0 * fabs(tmp - fst_in_model_time) / fst_in_model_time);
    }
    else
    {
        tmp = fst_total_pop * (T3_AVG);
        printf("Error: %.6lf%% \n", 100.0 * fabs(tmp - fst_out_model_time) / fst_out_model_time);
    }

    // --------------------------

    printf("\nTotal pushed (2-st queue): %zu elements\n", sec_total_push);
    printf("Total popped (2-st queue): %zu elements\n", sec_total_pop);
    printf("Average lifetime %.6lf\n", sec_total_lifetime / 10);

    if (T2_AVG - T4_AVG > -EPS)
        sec_model_time = sec_in_model_time;
    else
        sec_model_time = sec_out_model_time;
    printf("model time (2-st queue): %.6lf units\n", sec_model_time);
    if (sec_in_model_time > sec_out_model_time)
        printf("Downtime: %.6lf\n", sec_in_model_time - sec_out_model_time);

    tmp = sec_in_model_time / (T2_AVG);
    // printf("Calculated average elements count: %.6lf\n", tmp);
    printf("Error (in): %.6lf%% \n", 100.0 * fabs(tmp - sec_total_push) / min(tmp, sec_total_push));

    downtime = sec_in_model_time - sec_out_model_time;
    sec_model_time = downtime + sec_total_pop * (T4_AVG);
    printf("Error (out): %.6lf%% \n", 100.0 * fabs(sec_model_time - sec_in_model_time) / min(sec_model_time, sec_in_model_time));

    if (T2_AVG - T4_AVG > -EPS)
    {
        tmp = sec_total_push * (T2_AVG);
        printf("Error: %.6lf%% \n", 100.0 * fabs(tmp - sec_in_model_time) / sec_in_model_time);
    }
    else
    {
        tmp = sec_total_pop * (T4_AVG);
        printf("Error: %.6lf%% \n", 100.0 * fabs(tmp - sec_out_model_time) / sec_out_model_time);
    }

    // printf("\nModel time: %0.6lf units\n", model_time); // НО ТУТ ХЗ

    printf("\n--------------------------------");

    return total_time;
}

void list_push_by_delta_no_stats(list_queue_t *queue, double *T, double delta, const double T_min, const double T_max, const double T_prcs_min, const double T_prcs_max)
{
    while (delta - *T > -EPS)
    {
        list_push(rand_get(T_prcs_min, T_prcs_max), queue);

        delta -= *T;
        *T = rand_get(T_min, T_max);
    }

    *T -= delta;
}

void list_service_no_stats(list_queue_t *fst_queue, list_queue_t *sec_queue, double T1_MIN, double T1_MAX, double T2_MIN, double T2_MAX, double T3_MIN, double T3_MAX, double T4_MIN, double T4_MAX)
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

void time_comparing(static_queue_t *static_fst_queue, static_queue_t *static_sec_queue, list_queue_t *list_fst_queue, list_queue_t *list_sec_queue, double T1_MIN, double T1_MAX, double T2_MIN, double T2_MAX, double T3_MIN, double T3_MAX, double T4_MIN, double T4_MAX)
{
    double c = 0.0;

    long time_static = 0;
    long time_list = 0;

    size_t ss_mem_size = 0, ls_mem_size = 0;
    long ss_time_push = 0, ls_time_push = 0;
    long ss_time_pop = 0, ls_time_pop = 0;

    long ss_time_push_sum = 0, ls_time_push_sum = 0;
    long ss_time_pop_sum = 0, ls_time_pop_sum = 0;
    size_t elems = 0;

    long rand_time = 0.0;

    struct timespec t_beg, t_end;

    static_free(static_fst_queue);
    static_free(static_sec_queue);
    list_free(list_fst_queue);
    list_free(list_sec_queue);

    // TIME SERVICE

    for (size_t i = 0; i < TIME_COMPARING_ITER; i++)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        c = rand_get(0, 1);
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

        rand_time += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
    }

    for (size_t i = 0; i < TIME_COMPARING_ITER; i++)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        static_service_no_stats(static_fst_queue, static_sec_queue, T1_MIN, T1_MAX, T2_MIN, T2_MAX, T3_MIN, T3_MAX, T4_MIN, T4_MAX);
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

        static_free(static_fst_queue);
        static_free(static_sec_queue);

        time_static += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
    }

    for (size_t i = 0; i < TIME_COMPARING_ITER; i++)
    {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        list_service_no_stats(list_fst_queue, list_sec_queue, T1_MIN, T1_MAX, T2_MIN, T2_MAX, T3_MIN, T3_MAX, T4_MIN, T4_MAX);
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);

        list_free(list_fst_queue);
        list_free(list_sec_queue);

        time_list += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
    }

    static_free(static_fst_queue);
    static_free(static_sec_queue);
    list_free(list_fst_queue);
    list_free(list_sec_queue);

    printf("\nSTATISTICS (time in nsec) (memory in bytes) (STATIC QUEUE SIZE IS %d)\n", MAX_QUEUE_SIZE);

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

    for (size_t n_elems = 2; n_elems < MAX_QUEUE_SIZE; n_elems *= 2)
    {
        ss_time_push = 0;
        ls_time_push = 0;
        ss_time_pop = 0;
        ls_time_pop = 0;

        ss_mem_size = sizeof(static_queue_t);
        ls_mem_size = sizeof(list_queue_t) * n_elems + sizeof(list_queue_t *) + 2 * sizeof(queue_data_t);

        // // Чтобы адреса не портили результаты статистики
        // while (addresses_cap() < addresses_size() + n_elems)
        //     addresses_realloc();

        for (size_t i = 0; i < TIME_COMPARING_ITER; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            for (size_t i = 0; i < n_elems; i++)
                static_push('[', static_fst_queue);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
            ss_time_push += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            

            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            for (size_t i = 0; i < n_elems; i++)
                static_pop(&c, static_fst_queue);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
            ss_time_pop += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            
        }

        ss_time_push /= TIME_COMPARING_ITER;
        ss_time_pop /= TIME_COMPARING_ITER;

        for (size_t i = 0; i < TIME_COMPARING_ITER; i++)
        {
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            for (size_t i = 0; i < n_elems; i++)
                list_push('[', list_fst_queue);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
            ls_time_push += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            

            clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
            for (size_t i = 0; i < n_elems; i++)
                list_pop(&c, list_fst_queue);
            clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
            ls_time_pop += 1000000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec);
            
        }

        ls_time_push /= TIME_COMPARING_ITER;
        ls_time_pop /= TIME_COMPARING_ITER;

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

    printf("\nAverage service time (static): %.6lf", (double)time_static / TIME_COMPARING_ITER);
    printf("\nAverage push time (static): %.6lf", (double)ss_time_push_sum / elems);
    printf("\nAverage pop time (static):  %.6lf", (double)ss_time_pop_sum / elems);
    printf("\n\nAverage service time (list): %.6lf", (double)time_list / TIME_COMPARING_ITER);
    printf("\nAverage push time (list):  %.6lf", (double)ls_time_push_sum / elems);
    printf("\nAverage pop time (list): %.6lf\n", (double)ls_time_pop_sum / elems);

    printf("\nRand time: %ld\n\n", rand_time / TIME_COMPARING_ITER);

    if (ss_time_push_sum < ls_time_push_sum)
        printf("Static PUSH time is better than list on: %0.6lf%%\n", (double)ls_time_push_sum / (double)ss_time_push_sum * 100.0);
    else if (ss_time_push_sum < ls_time_push_sum)
        printf("List PUSH time is better than static on: %0.6lf%%\n", (double)ss_time_push_sum / (double)ls_time_push_sum * 100.0);
    else 
        printf("List PUSH time and static PUSH time are equal\n");

    if (ss_time_pop_sum < ls_time_pop_sum)
        printf("Static POP time is better than list on: %0.6lf%%\n", (double)ls_time_pop_sum / (double)ss_time_pop_sum * 100.0);
    else if (ss_time_pop_sum < ls_time_pop_sum)
        printf("List POP time is better than static on: %0.6lf%%\n", (double)ss_time_pop_sum / (double)ls_time_pop_sum * 100.0);
    else 
        printf("List POP time and static POP time are equal\n");

    static_free(static_fst_queue);
    list_free(list_fst_queue);
}
