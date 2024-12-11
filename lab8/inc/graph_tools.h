#ifndef GRAPH_TOOLS_H__

#define GRAPH_TOOLS_H__

#include <stddef.h>

typedef enum 
{
    GRAPH_OK = 0,
    GRAPH_ERR_ALLOC,
    GRAPH_ERR_UNKNOWN_CITY
} graph_error_t;

typedef struct
{
    city_t *beg;
    city_t *end;
    
    double value;
    double fee;
} lane_t;

typedef struct
{
    char *name;

    double min_value;
    double min_fee;
} city_t;

typedef struct
{
    city_t *cities;
    size_t n;

    lane_t ***lanes;
} graph_t;


#endif
