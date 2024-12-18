#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graph_tools.h"

#define EPS 1e-8

void str_free(char **str, size_t *size)
{
    free(*str);
    *str = NULL;

    *size = 0;
}

// ------------------------------

void cities_free(city_t ***cities, size_t n)
{
    city_t **tmp = *cities;

    for (size_t i = 0; i < n; i++)
        if (tmp[i])
        {
            free(tmp[i]->name);
            free(tmp[i]);
            tmp[i] = NULL;
        }

    free(tmp);
    *cities = NULL;
}

void lanes_free(lane_t ****lanes, size_t n)
{
    lane_t ***tmp = *lanes;

    for (size_t i = 0; i < n; i++)
    {
        if (tmp[i])
        {
            for (size_t j = 0; j < n; j++)
                if (tmp[i][j])
                    free(tmp[i][j]);
            
            free(tmp[i]);
        }
    }

    free(tmp);
    *lanes = NULL;
}

void graph_free(graph_t **graph)
{
    graph_t *tmp;

    if (! graph || ! *graph)
        return;

    tmp = *graph;

    if (tmp->cities)
        cities_free(&tmp->cities, tmp->n);
    if (tmp->lanes)
        lanes_free(&tmp->lanes, tmp->n);

    free(tmp);
    *graph = NULL;
}

void graph_way_color(graph_t *graph, size_t end_i)
{
    if (! graph)
        return;

    for (city_t *cur = graph->cities[end_i]; cur; cur = cur->last)
        cur->color = COLOR_RED;
}

// ------------------------------

void graph_clear(graph_t *graph)
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

// ------------------------------

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

// ------------------------------

void graph_output_all_lanes(graph_t *graph, FILE *f)
{
    for (size_t i = 0; i < graph->n; i++)
    {
        for (size_t j = 0; j < graph->n; j++)
            if (graph->lanes[i][j])
            {
                fprintf(f, "  \"");
                fputs(graph->cities[i]->name, f);
                fprintf(f, "\" -> \"");
                fputs(graph->cities[j]->name, f);
                fprintf(f, "\"");
                fprintf(f, "[label=\"(%.1lf %.1lf)\", ", graph->lanes[i][j]->value, graph->lanes[i][j]->fee);
                fprintf(f, "]\n");
            }
    }
}

void graph_output_additional_find_path(graph_t *graph, FILE *f)
{
    for (size_t i = 0; i < graph->n; i++)
    {
        for (size_t j = 0; j < graph->n; j++)
            if (graph->lanes[i][j])
            {
                fprintf(f, "  \"");
                fputs(graph->cities[i]->name, f);
                fprintf(f, "\" -> \"");
                fputs(graph->cities[j]->name, f);
                fprintf(f, "\"");
                fprintf(f, "[label=\"(%.1lf %.1lf)\", ", graph->lanes[i][j]->value, graph->lanes[i][j]->fee);

                if (graph->cities[j]->last == graph->cities[i])
                {
                    if (graph->cities[j]->color == COLOR_RED)
                        fprintf(f, "color=red");
                }
                
                fprintf(f, "]\n");
            }
    }
}

void graph_output(graph_t *graph, void (*additional_output)(graph_t *, FILE *))
{
    FILE *f = NULL;
    char *filename = "TreeVisual.gv";

    f = fopen(filename, "w");

    fprintf(f, "digraph \"");
    fputs(filename, f);
    fprintf(f, "\" {\n");
    fprintf(f, "  node [fontdata=\"Arial\"];\n");

    for (size_t i = 0; i < graph->n; i++)
    {
        fprintf(f, "  \"");
        fputs(graph->cities[i]->name, f);
        fprintf(f, "\"");

        if (graph->cities[i]->color == COLOR_RED)
            fprintf(f, "[color=red]\n");
        else
            fputc('\n', f);
    }

    if (additional_output)
        additional_output(graph, f);
    else
        graph_output_all_lanes(graph, f);

    fprintf(f, "}");

    fclose(f);

    system("dot -Tpng TreeVisual.gv -o graph.png");
}

// ------------------------------

graph_error_t graph_way_find_path(graph_t *graph, char *beg, char *end)
{
    size_t iters;
    size_t last_i = 0;
    size_t cur_i = 0;
    size_t end_i = 0;

    double value = 0.0;
    double fee = 0.0;
    double key = 0.0;

    double min_key; // 55555

    if ((cur_i = city_find_by_str(graph, beg)) == graph->n)
        return GRAPH_ERR_UNKNOWN_CITY;

    last_i = cur_i;

    if ((end_i = city_find_by_str(graph, end)) == graph->n)
        return GRAPH_ERR_UNKNOWN_CITY;

    graph_clear(graph);

    graph->cities[cur_i]->min_value = 0;
    graph->cities[cur_i]->min_fee = 0;
    graph->cities[cur_i]->is_major = MAJOR;

    iters = 0;
    while (iters < graph->n && ! graph->cities[end_i]->is_major)
    {
        for (size_t j = 0; j < graph->n; j++)
            if (graph->lanes[cur_i][j] && j != last_i)
            {
                value = graph->cities[cur_i]->min_value + graph->lanes[cur_i][j]->value;
                fee = graph->cities[cur_i]->min_fee + graph->lanes[cur_i][j]->fee;
                key = value + fee; // 55555

                if (fabs(graph->cities[j]->min_value - (-1)) < EPS)
                {
                    graph->cities[j]->min_value = value;
                    graph->cities[j]->min_fee = fee;
                    graph->cities[j]->last = graph->cities[cur_i];
                }
                else if (key - graph->cities[j]->min_value - graph->cities[j]->min_fee < -EPS) // 55555
                {
                    graph->cities[j]->min_value = value;
                    graph->cities[j]->min_fee = fee;
                    graph->cities[j]->last = graph->cities[cur_i];
                }
            }

        last_i = cur_i;
        
        min_key = -1.0;
        for (size_t i = 0; i < graph->n; i++)
        {
            value = graph->cities[i]->min_value;
            fee = graph->cities[i]->min_fee;
            key = value + fee; // 55555

            if (fabs(value = graph->cities[i]->min_value + 1) > EPS && ! graph->cities[i]->is_major && (fabs(min_key + 1.0) < EPS || min_key - key > EPS)) // 55555
            {
                min_key = key;
                cur_i = i;
            }
        }

        graph->cities[cur_i]->is_major = MAJOR;
        iters++;
    }

    key = graph->cities[end_i]->min_value + graph->cities[end_i]->min_fee;
    
    if (graph->cities[end_i]->min_value < EPS)
            graph->cities[end_i]->min_value = 0.0;
    if (graph->cities[end_i]->min_fee < EPS)
            graph->cities[end_i]->min_fee = 0.0;

    graph_way_color(graph, end_i);

    if (key < -EPS)
        printf("\nPATH ISN'T EXIST\n");
    else
    {
        printf("\nMIN PATH S+P: %.6lf\n", key);
        printf("S: %.6lf P: %.6lf\n", graph->cities[end_i]->min_value, graph->cities[end_i]->min_fee);
    }

    graph_output(graph, graph_output_additional_find_path);

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
    int flag = 1;

    graph_t *graph_tmp = NULL;
    city_t **cities_tmp = NULL;
    city_t *city_tmp = NULL;
    size_t n = 0;
    city_t **realloc_tmp = NULL;

    char *name_tmp = NULL;
    char *p = NULL;
    size_t size;

    double num_tmp = 0.0;
    lane_t ***lanes_tmp = NULL;
    lane_t **lane_stroke_tmp = NULL;
    lane_t *lane_element_tmp = NULL;

    if (! (f = fopen(filename, "r")))
        return GRAPH_ERR_INVALID_FILE;

    if (! graph)
    {
        fclose(f);

        return GRAPH_ERR_INVALID_IN;
    }

    if (! (graph_tmp = memory_alloc(1, sizeof(graph_t))))
    {
        fclose(f);

        return GRAPH_ERR_ALLOC;
    }

    while (getline(&name_tmp, &size, f) != -1 && strcmp(name_tmp, READ_END))
    {
        if (! (realloc_tmp = realloc(cities_tmp, (n + 1) * sizeof(city_t *))))
        {
            fclose(f);
            free(graph_tmp);
            cities_free(&cities_tmp, n);
            free(name_tmp);

            return GRAPH_ERR_ALLOC;
        }
        cities_tmp = realloc_tmp;

        if ((p = strchr(name_tmp, '\n')))
            *p = '\0';

        if (! (city_tmp = memory_alloc(1, sizeof(struct city))))
        {
            fclose(f);
            free(graph_tmp);
            cities_free(&cities_tmp, n);
            free(name_tmp);

            return GRAPH_ERR_ALLOC;
        }
        city_tmp->name = name_tmp;

        cities_tmp[n] = city_tmp;

        str_unpin(&name_tmp, &size);

        n++;
    }

    if (name_tmp)
    {
        free(name_tmp);
        name_tmp = NULL;
    }

    if (! n)
    {
        fclose(f);
        free(graph_tmp);
        cities_free(&cities_tmp, n);

        return GRAPH_ERR_NO_DATA;
    }

    if (n < 2)
    {
        fclose(f);
        free(graph_tmp);
        cities_free(&cities_tmp, n);

        return GRAPH_ERR_INVALID_DATA;
    }

    if (! (lanes_tmp = memory_alloc(n, sizeof(lane_t **))))
    {
        fclose(f);
        free(graph_tmp);
        cities_free(&cities_tmp, n);

        return GRAPH_ERR_INVALID_DATA;
    }

    for (size_t i = 0; i < n; i++)
    {
        if (! (lane_stroke_tmp = memory_alloc(n, sizeof(lane_t *))))
        {
            fclose(f);
            free(graph_tmp);
            cities_free(&cities_tmp, n);
            lanes_free(&lanes_tmp, n);

            return GRAPH_ERR_ALLOC;
        }

        lanes_tmp[i] = lane_stroke_tmp;
    }

    for (size_t i = 0; flag && i < n - 1; i++)
        for (size_t j = i + 1; flag && j < n; j++)
        {
            if (fscanf(f, "%lf", &num_tmp) != 1)
            {
                if (! feof(f) || ferror(f))
                {
                    fclose(f);
                    free(graph_tmp);
                    cities_free(&cities_tmp, n);
                    lanes_free(&lanes_tmp, n);

                    return GRAPH_ERR_INVALID_DATA;
                }
            }
            else
            {
                if (! (num_tmp < EPS))
                {
                    if (! (lane_element_tmp = memory_alloc(1, sizeof(lane_t))))
                    {
                        fclose(f);
                        free(graph_tmp);
                        cities_free(&cities_tmp, n);
                        lanes_free(&lanes_tmp, n);

                        return GRAPH_ERR_ALLOC;
                    }

                    lane_element_tmp->value = num_tmp;
                    lanes_tmp[i][j] = lane_element_tmp;
                    lane_element_tmp = NULL;

                    if (fscanf(f, "%lf", &num_tmp) != 1 || num_tmp < EPS)
                    {
                        fclose(f);
                        free(graph_tmp);
                        cities_free(&cities_tmp, n);
                        lanes_free(&lanes_tmp, n);
                        free(lane_element_tmp);

                        return GRAPH_ERR_INVALID_DATA;
                    }

                    if (! (lane_element_tmp = memory_alloc(1, sizeof(lane_t))))
                    {
                        fclose(f);
                        free(graph_tmp);
                        cities_free(&cities_tmp, n);
                        lanes_free(&lanes_tmp, n);

                        return GRAPH_ERR_ALLOC;
                    }

                    lane_element_tmp->value = num_tmp;
                    lanes_tmp[j][i] = lane_element_tmp;
                    lane_element_tmp = NULL;

                    if (fscanf(f, "%lf", &num_tmp) != 1 || num_tmp < -EPS)
                    {
                        fclose(f);
                        free(graph_tmp);
                        cities_free(&cities_tmp, n);
                        lanes_free(&lanes_tmp, n);
                        free(lane_element_tmp);

                        return GRAPH_ERR_INVALID_DATA;
                    }
                    else
                    {
                        lanes_tmp[i][j]->fee = num_tmp;
                        lanes_tmp[j][i]->fee = num_tmp;
                    }
                }
            }
        }

    fclose(f);

    graph_tmp->cities = cities_tmp;
    graph_tmp->lanes = lanes_tmp;
    graph_tmp->n = n;

    *graph = graph_tmp;

    return GRAPH_OK;
}