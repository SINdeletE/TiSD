#include <stdio.h>
#include <string.h>

#include "apart_tools.h"

char *address(apart_t *apart)
{
    return apart->address;
}

double square(apart_t *apart)
{
    return apart->square;
}

double *p_square(apart_t *apart)
{
    return &apart->square;
}

int room_count(apart_t *apart)
{
    return apart->room_count;
}

int *p_room_count(apart_t *apart)
{
    return &apart->room_count;
}

double quad_meter_cost(apart_t *apart)
{
    return apart->quad_meter_cost;
}

double *p_quad_meter_cost(apart_t *apart)
{
    return &apart->quad_meter_cost;
}

bool is_primary(apart_t *apart)
{
    return apart->is_primary;
}

bool *p_is_primary(apart_t *apart)
{
    return &apart->is_primary;
}

bool prim_is_with_decoration(apart_t *apart)
{
    return apart->info.primary.is_with_decoration;
}

bool *p_prim_is_with_decoration(apart_t *apart)
{
    return &apart->info.primary.is_with_decoration;
}

int sec_build_year(apart_t *apart)
{
    return apart->info.secondary.build_year;
}

int *p_sec_build_year(apart_t *apart)
{
    return &apart->info.secondary.build_year;
}

int sec_owner_count(apart_t *apart)
{
    return apart->info.secondary.owner_count;
}

int *p_sec_owner_count(apart_t *apart)
{
    return &apart->info.secondary.owner_count;
}

bool sec_were_there_animals(apart_t *apart)
{
    return apart->info.secondary.were_there_animals;
}

bool *p_sec_were_there_animals(apart_t *apart)
{
    return &apart->info.secondary.were_there_animals;
}

void apart_output(apart_t *apart)
{
    printf("\nAddress: ");
    fputs(address(apart), stdout);
    printf("\n");

    printf("Square: %lf\n", square(apart));
    printf("Room count: %d\n", room_count(apart));
    printf("Quad meter cost: %lf\n", quad_meter_cost(apart));
    
    if (is_primary(apart))
    {
        printf("PRIMARY\n");

        printf("Is with decoration: ");
        if (prim_is_with_decoration(apart))
            printf("Yes\n");
        else
            printf("No\n");
    }
    else
    {
        printf("SECONDARY\n");

        printf("Build year: %d\n", sec_build_year(apart));
        printf("Owner count: %d\n", sec_owner_count(apart));

        printf("Were there animals: ");
        if (sec_were_there_animals(apart))
            printf("Yes\n");
        else
            printf("No\n");
    }
}

int apart_fill(apart_t *apart)
{
    char tmp[STR_SIZE + 1] = "";

    // ADDRESS
    printf("Enter address: ");
    if (string_read(apart->address, STR_SIZE + 1, stdin))
        return APART_FILL_ERR_WITHOUT_BUF;

    // SQUARE
    printf("Enter square: ");
    if (fscanf(stdin, "%lf", p_square(apart)) != 1)
        return APART_FILL_ERR_BUF;

    if (clear_buf(stdin))
        return APART_FILL_ERR_BUF;

    if (square(apart) < EPS)
        return APART_FILL_ERR_WITHOUT_BUF;

    // ROOM COUNT
    printf("Enter room count: ");
    if (fscanf(stdin, "%d", p_room_count(apart)) != 1)
        return APART_FILL_ERR_BUF;

    if (clear_buf(stdin))
        return APART_FILL_ERR_BUF;

    if (room_count(apart) <= 0)
        return APART_FILL_ERR_WITHOUT_BUF;

    // QUAD_METER_COST
    printf("Enter quad meter cost: ");
    if (fscanf(stdin, "%lf", p_quad_meter_cost(apart)) != 1)
        return APART_FILL_ERR_BUF;

    if (clear_buf(stdin))
        return APART_FILL_ERR_BUF;

    if (quad_meter_cost(apart) < -EPS)
        return APART_FILL_ERR_WITHOUT_BUF;

    // PRIMARY/SECONDARY
    printf("Enter apart type (enter \"primary\" or \"secondary\"): ");
    if (string_read(tmp, STR_SIZE + 1, stdin))
        return APART_FILL_ERR_WITHOUT_BUF;

    if (strcmp(tmp, "primary") == 0)
    {
        *(p_is_primary(apart)) = true;

        printf("Is this apart with decoration? (yes/no): ");
        if (string_read(tmp, STR_SIZE + 1, stdin))
            return APART_FILL_ERR_WITHOUT_BUF;

        if (strcmp(tmp, "yes") == 0)
            *(p_prim_is_with_decoration(apart)) = true;
        else if (strcmp(tmp, "no") == 0)
            *(p_prim_is_with_decoration(apart)) = false;
        else
            return APART_FILL_ERR_WITHOUT_BUF;
    }
    else if (strcmp(tmp, "secondary") == 0)
    {
        *(p_is_primary(apart)) = false;

        // BUILD_YEAR
        printf("Enter build year: ");
        if (fscanf(stdin, "%d", p_sec_build_year(apart)) != 1)
            return APART_FILL_ERR_BUF;

        if (clear_buf(stdin))
            return APART_FILL_ERR_BUF;

        if (sec_build_year(apart) < 0)
            return APART_FILL_ERR_WITHOUT_BUF;

        // OWNER_COUNT
        printf("Enter owner count: ");
        if (fscanf(stdin, "%d", p_sec_owner_count(apart)) != 1)
            return APART_FILL_ERR_BUF;

        if (clear_buf(stdin))
            return APART_FILL_ERR_BUF;

        if (sec_owner_count(apart) < 0)
            return APART_FILL_ERR_WITHOUT_BUF;

        // WERE_THERE_ANIMALS
        printf("Were there animals? (yes/no): ");
        if (string_read(tmp, STR_SIZE + 1, stdin))
            return APART_FILL_ERR_WITHOUT_BUF;

        if (strcmp(tmp, "yes") == 0)
            *(p_sec_were_there_animals(apart)) = true;
        else if (strcmp(tmp, "no") == 0)
            *(p_sec_were_there_animals(apart)) = false;
        else
            return APART_FILL_ERR_WITHOUT_BUF;
    }
    else
        return APART_FILL_ERR_WITHOUT_BUF;

    return APART_FILL_OK;
}

void apart_write(FILE *f, apart_t *apart)
{
    fputs(address(apart), f);
    fprintf(f, "\n");

    fprintf(f, "%lf\n", square(apart));
    fprintf(f, "%d\n", room_count(apart));
    fprintf(f, "%lf\n", quad_meter_cost(apart));
    
    if (is_primary(apart))
    {
        fprintf(f, "primary\n");

        if (prim_is_with_decoration(apart))
            fprintf(f, "yes\n");
        else
            fprintf(f, "no\n");
    }
    else
    {
        fprintf(f, "secondary\n");

        fprintf(f, "%d\n", sec_build_year(apart));
        fprintf(f, "%d\n", sec_owner_count(apart));

        if (sec_were_there_animals(apart))
            fprintf(f, "yes\n");
        else
            fprintf(f, "no\n");
    }
}

// KEY

int *key_p_room_count(keystat_t *key)
{
    return &key->room_count;
}

// KEY
