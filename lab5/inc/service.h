#ifndef SERVICE_H__

#define SERVICE_H__

#include "static_queue_tools.h"
#include "list_queue_tools.h"

#define MAX_PRCS_COUNT 1000
#define INFO_PRCS_COUNT 100

#define TIME_COMPARING_ITER 1000

double static_service(static_queue_t *fst_queue, static_queue_t *sec_queue, double T1_MIN, double T1_MAX, double T2_MIN, double T2_MAX, double T3_MIN, double T3_MAX, double T4_MIN, double T4_MAX);
double list_service(list_queue_t *fst_queue, list_queue_t *sec_queue, double T1_MIN, double T1_MAX, double T2_MIN, double T2_MAX, double T3_MIN, double T3_MAX, double T4_MIN, double T4_MAX);

void static_service_no_stats(static_queue_t *fst_queue, static_queue_t *sec_queue, double T1_MIN, double T1_MAX, double T2_MIN, double T2_MAX, double T3_MIN, double T3_MAX, double T4_MIN, double T4_MAX);
void list_service_no_stats(list_queue_t *fst_queue, list_queue_t *sec_queue, double T1_MIN, double T1_MAX, double T2_MIN, double T2_MAX, double T3_MIN, double T3_MAX, double T4_MIN, double T4_MAX);

void time_comparing(static_queue_t *static_fst_queue, static_queue_t *static_sec_queue, list_queue_t *list_fst_queue, list_queue_t *list_sec_queue, double T1_MIN, double T1_MAX, double T2_MIN, double T2_MAX, double T3_MIN, double T3_MAX, double T4_MIN, double T4_MAX);

#endif
