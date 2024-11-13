#ifndef SERVICE_H__

#define SERVICE_H__

#include "static_queue_tools.h"
#include "list_queue_tools.h"

#define T1_MIN 1
#define T1_MAX 5

#define T2_MIN 0
#define T2_MAX 3

#define T3_MIN 0
#define T3_MAX 4

#define T4_MIN 0
#define T4_MAX 1

#define T1_AVG (T1_MAX + T1_MIN) / 2
#define T2_AVG (T2_MAX + T2_MIN) / 2
#define T3_AVG (T3_MAX + T3_MIN) / 2
#define T4_AVG (T4_MAX + T4_MIN) / 2

#define MAX_PRCS_COUNT 1000
#define INFO_PRCS_COUNT 100

#define TIME_COMPARING_ITER 500

double static_service(static_queue_t *fst_queue, static_queue_t *sec_queue);
double list_service(list_queue_t *fst_queue, list_queue_t *sec_queue);

void static_service_no_stats(static_queue_t *fst_queue, static_queue_t *sec_queue);
void list_service_no_stats(list_queue_t *fst_queue, list_queue_t *sec_queue);

void time_comparing(static_queue_t *static_fst_queue, static_queue_t *static_sec_queue, list_queue_t *list_fst_queue, list_queue_t *list_sec_queue);

#endif
