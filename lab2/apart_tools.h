#ifndef APART_TOOLS_H__

#define APART_TOOLS_H__

#include <stdbool.h>

#include "string_tools.h"

#define APART_FILL_OK 0
#define APART_FILL_ERR_BUF 1
#define APART_FILL_ERR_WITHOUT_BUF 2

#define EPS 1e-8

typedef struct
{
    bool is_with_decoration;
} primary_t;

typedef struct
{
    int build_year;
    int owner_count;
    bool were_there_animals;
} secondary_t;

typedef union
{
    primary_t primary;
    secondary_t secondary;
} apart_info_t;

typedef struct
{
    char address[STR_SIZE + 1];
    double square;
    int room_count;
    double quad_meter_cost;

    bool is_primary;
    apart_info_t info;
} apart_t;

// KEY

typedef struct
{
    size_t index;
    int room_count;
} keystat_t;

// KEY

char *address(apart_t *apart);

double square(apart_t *apart);
double *p_square(apart_t *apart);

int room_count(apart_t *apart);
int *p_room_count(apart_t *apart);

double quad_meter_cost(apart_t *apart);
double *p_quad_meter_cost(apart_t *apart);

bool is_primary(apart_t *apart);
bool *p_is_primary(apart_t *apart);

bool prim_is_with_decoration(apart_t *apart);
bool *p_prim_is_with_decoration(apart_t *apart);

int sec_build_year(apart_t *apart);
int *p_sec_build_year(apart_t *apart);

int sec_owner_count(apart_t *apart);
int *p_sec_owner_count(apart_t *apart);

bool sec_were_there_animals(apart_t *apart);
bool *p_sec_were_there_animals(apart_t *apart);

void apart_output(apart_t *apart);
int apart_fill(apart_t *apart);

void apart_write(FILE *f, apart_t *apart);

// KEY 

int *key_p_room_count(keystat_t *key);

// KEY

#endif
