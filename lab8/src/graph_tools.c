#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graph_tools.h"

#define EPS 1e-8

// ------------------------------

void cities_free(city_t ***cities, size_t n)
{
    city_t **tmp = cities;

    for (size_t i = 0; i < n; i++)
    {
        free(cities[i]->name);
        free(cities[i]);
        cities[i] = NULL;
    }

    free(tmp);
    *cities = NULL;
}

// ------------------------------

void way_find_clear(graph_t *graph)
{
    for (size_t i = 0; i < graph->n; i++)
    {
        graph->cities[i]->is_major = 0;
        graph->cities[i]->last = NULL;
        graph->cities[i]->color = COLOR_NONE;

        graph->cities[i]->min_value = -1;
        graph->cities[i]->min_fee = -1;
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
        if (! strcmp(graph->cities[i]->name, str))
        {
            tmp = i;

            flag = 0;
        }

    return tmp;
}

graph_error_t way_find(graph_t *graph, char *beg, char *end)
{
    size_t iters;
    size_t cur_i = 0;
    size_t end_i = 0;

    double value = 0.0;
    double fee = 0.0;
    double key = 0.0;

    double min_key; // 55555

    if ((cur_i = city_find_by_str(graph, beg)) == graph->n)
        return GRAPH_ERR_UNKNOWN_CITY;

    if ((end_i = city_find_by_str(graph, end)) == graph->n)
        return GRAPH_ERR_UNKNOWN_CITY;

    way_find_clear(graph);

    graph->cities[cur_i]->min_value = 0;
    graph->cities[cur_i]->min_fee = 0;
    graph->cities[cur_i]->is_major = MAJOR;

    iters = 0;
    while (iters < graph->n && ! graph->cities[end_i]->is_major)
    {
        for (size_t j = 0; j < graph->n; j++)
            if (graph->lanes[cur_i][j])
            {
                value = graph->cities[cur_i]->min_value + graph->lanes[cur_i][j]->value;
                fee = graph->cities[cur_i]->min_fee + graph->lanes[cur_i][j]->fee;
                key = value + fee; // 55555

                if (fabs(graph->cities[j]->min_value - (-1)) < EPS)
                {
                    graph->cities[j]->min_value = value;
                    graph->cities[j]->min_fee = fee;
                }
                else if (key - graph->cities[j]->min_value - graph->cities[j]->min_fee < -EPS) // 55555
                {
                    graph->cities[j]->min_value = value;
                    graph->cities[j]->min_fee = fee;
                }
            }
        
        min_key = -1.0;
        for (size_t i = 0; i < graph->n; i++)
        {
            value = graph->cities[i]->min_value;
            fee = graph->cities[i]->min_fee;
            key = value + fee; // 55555

            if (! graph->cities[i]->is_major && min_key - key < -EPS) // 55555
            {
                min_key = key;
                cur_i = i;
            }
        }

        graph->cities[cur_i]->is_major = MAJOR;
        iters++;
    }

    return GRAPH_OK;
}

// ------------------------------

void str_unpin(char **str, size_t *size)
{
    *str = NULL;
    *size = 0;
}

graph_error_t graph_read_from_file(graph_t **graph, char *filename)
{
    FILE *f = NULL;

    graph_t *graph_tmp = NULL;
    city_t **cities_tmp = NULL;
    size_t n = 0;
    city_t *realloc_tmp = NULL;

    char *name_tmp = NULL;
    char *p = NULL;
    size_t size;

    double num_tmp = 0.0;
    lane_t *lane_tmp = NULL;

    if (! (f = fopen(filename, "r")))
        return GRAPH_ERR_INVALID_FILE;

    if (! graph)
        return GRAPH_ERR_INVALID_IN;

    if (! (graph_tmp = memory_alloc(1, sizeof(graph_t))))
        return GRAPH_ERR_ALLOC;

    while (getline(&name_tmp, &size, f) != -1 && strcmp(name_tmp, READ_END))
    {
        if (! (realloc_tmp = realloc(cities_tmp, (n + 1) * sizeof(city_t *))))
        {
            free(graph_tmp);
            cities_free(&cities_tmp, n);

            return GRAPH_ERR_ALLOC;
        }

        if (p = strchr(name_tmp, '\n'))
            *p = '\0';

        str_unpin(&name_tmp, &size);

        n++;
    }

    if (! n)
    {
        free(graph_tmp);
        cities_free(&cities_tmp, n);

        return GRAPH_ERR_NO_DATA;
    }

    if (! (graph_tmp = memory_alloc(1, sizeof(graph_t))))
        return GRAPH_ERR_ALLOC;

    fclose(f);

    return GRAPH_OK;
}