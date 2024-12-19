#ifndef GRAPH_TOOLS_H__

#define GRAPH_TOOLS_H__

#include <stddef.h>

#define MAJOR 1

#define READ_END "\n"

typedef enum 
{
    GRAPH_OK = 0,

    // GRAPH_WAY FIND ERRORS
    GRAPH_ERR_UNKNOWN_CITY,

    // READ FILE ERRORS
    GRAPH_ERR_INVALID_IN,
    GRAPH_ERR_INVALID_DATA,
    GRAPH_ERR_INVALID_FILE,
    GRAPH_ERR_NO_DATA,

    // GENERAL ERRORS
    GRAPH_ERR_ALLOC,
} graph_error_t;

typedef enum
{
    COLOR_NONE = 0,
    COLOR_RED = 1
} color_t;

typedef struct
{
    double value;
    double fee;
} lane_t;

typedef struct city city_t;
struct city
{
    char *name;

    int is_major;
    city_t *last;
    color_t color;

    double min_value;
    double min_fee;
};

typedef struct
{
    city_t **cities;
    size_t n;

    lane_t ***lanes;
} graph_t;

void graph_free(graph_t **graph);
void str_unpin(char **str, size_t *size);
void str_free(char **str, size_t *size);

void graph_clear(graph_t *graph);
graph_error_t graph_way_find_path(graph_t *graph, char *beg, char *end);
graph_error_t graph_read_from_file(graph_t **graph, char *filename);
void graph_output(graph_t *graph, void (*additional_output)(graph_t *, FILE *));

#endif
