#include <stdlib.h>
#include <string.h>

#include "graph_tools.h"

// ------------------------------

void way_find_clear(graph_t *graph)
{
    for (size_t i = 0; i < graph->n; i++)
    {
        graph->cities[i].min_value = 0;
        graph->cities[i].min_fee = 0;
    }
}

void *memory_alloc(size_t n, size_t size)
{
    void *tmp = NULL;

    tmp = calloc(n, size);

    return tmp;
}

size_t city_find_by_str(graph_t *graph, char *str)
{
    int flag = 1;
    size_t tmp = graph->n;

    for (size_t i = 0; flag && i < graph->n; i++)
        if (! strcmp(graph->cities[i].name, str))
        {
            tmp = i;

            flag = 0;
        }

    return tmp;
}

graph_error_t way_find(graph_t *graph, char *beg, char *end)
{
    size_t beg_id = 0, end_id = 0;

    int *checked_city_bool = NULL;

    lane_t **lane_set = NULL;
    size_t lane_set_size = 0;
    lane_t *min_lane = NULL;

    size_t cur_city_index = 0;

    if ((beg_id = city_find_by_str(graph, beg)) == graph->n)
        return GRAPH_ERR_UNKNOWN_CITY;

    if ((end_id = city_find_by_str(graph, end)) == graph->n)
        return GRAPH_ERR_UNKNOWN_CITY;

    if (! (checked_city_bool = memory_alloc(graph->n, sizeof(int))))
        return GRAPH_ERR_ALLOC;

    if (! (lane_set = memory_alloc(graph->n * graph->n, sizeof(lane_t *))))
    {
        free(checked_city_indexes);

        return GRAPH_ERR_ALLOC;
    }

    way_find_clear();

    
}

// ------------------------------
