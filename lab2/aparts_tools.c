#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#include "string_tools.h"
#include "aparts_tools.h"

#define ITER_COUNT 100

int apart_read(apart_t *apart, FILE *f);
bool file_is_incorrect(FILE *f, size_t *size);

void aparts_ncpy(apart_t *dst, apart_t *src, size_t n);
void aparts_write(FILE *f, apart_t *aparts, size_t size);

void *shift_inc(void *p, size_t shift);
void *shift_dec(void *p, size_t shift);
void assign(void *src, void *dst, size_t size);
void swap(void *src, void *dst, size_t size);

// KEY

void key_fill(keystat_t *key, int room_count, size_t index);
void keys_ncpy(keystat_t *dst, keystat_t *src, size_t n);
void stat_output(long time_insertion_aparts, long time_gnome_aparts, long time_insertion_keys, long time_gnome_keys, size_t size);

// KEY

int aparts_read(apart_t **aparts, size_t *size)
{
    FILE *f = NULL;
    char filename[STR_SIZE + 1] = "";
    
    size_t last_size = *size;

    *size = 0;

    if (string_read(filename, STR_SIZE + 1, stdin))
        return READ_ERR_INVALID_FILENAME;

    if ((f = fopen(filename, "r")) == NULL)
        return READ_ERR_NO_FILE;

    if (file_is_incorrect(f, size))
    {
        fclose(f);
        *size = last_size;

        return READ_ERR_INVALID_FILE;
    }

    if (*size == 0)
    {
        fclose(f);

        return READ_ERR_NO_DATA;
    }

    // READY TO MALLOC: IF MALLOC GREATER THAN ONE
    if (*aparts != NULL)
        free(*aparts);

    // MALLOC
    if ((*aparts = malloc(*size * sizeof(apart_t))) == NULL)
    {
        fclose(f);

        return READ_ERR_DATA_ALLOC;
    }

    for (size_t i = 0; i < *size; i++)
        if (apart_read(&(*aparts)[i], f))
        {
            fclose(f);

            free(*aparts);
            *aparts = NULL;

            return READ_ERR_INVALID_APART;
        }

    fclose(f);

    return READ_OK;
}

bool file_is_incorrect(FILE *f, size_t *size)
{
    apart_t tmp;

    bool flag = true;
    int code = 0;

    *size = 0;

    while (flag)
    {
        code = apart_read(&tmp, f);

        if (code == APART_READ_ERR)
            return true;
        else if (code == APART_READ_EOF)
            flag = false;
        else
            (*size)++;
    }

    rewind(f);

    return false;
}

int apart_read(apart_t *apart, FILE *f)
{
    char tmp[STR_SIZE + 1] = "";

    // ADDRESS
    if (string_read(apart->address, STR_SIZE + 1, f))
    {
        if (feof(f))
            return APART_READ_EOF;
        else if (ferror(f))
            return APART_READ_ERR;
    }

    // SQUARE
    if (fscanf(f, "%lf", p_square(apart)) != 1)
        return APART_READ_ERR;

    if (clear_buf(f) || square(apart) < EPS)
        return APART_READ_ERR;

    // ROOM COUNT
    if (fscanf(f, "%d", p_room_count(apart)) != 1)
        return APART_READ_ERR;

    if (clear_buf(f) || room_count(apart) <= 0)
        return APART_READ_ERR;

    // QUAD_METER_COST
    if (fscanf(f, "%lf", p_quad_meter_cost(apart)) != 1)
        return APART_READ_ERR;

    if (clear_buf(f) || quad_meter_cost(apart) < -EPS)
        return APART_READ_ERR;

    // PRIMARY/SECONDARY
    if (string_read(tmp, STR_SIZE + 1, f))
        return APART_READ_ERR;

    if (strcmp(tmp, "primary") == 0)
    {
        *(p_is_primary(apart)) = true;

        if (string_read(tmp, STR_SIZE + 1, f))
            return APART_READ_ERR;

        if (strcmp(tmp, "yes") == 0)
            *(p_prim_is_with_decoration(apart)) = true;
        else if (strcmp(tmp, "no") == 0)
            *(p_prim_is_with_decoration(apart)) = false;
        else
            return APART_READ_ERR;
    }
    else if (strcmp(tmp, "secondary") == 0)
    {
        *(p_is_primary(apart)) = false;

        // BUILD_YEAR
        if (fscanf(f, "%d", p_sec_build_year(apart)) != 1)
            return APART_READ_ERR;

        if (clear_buf(f) || sec_build_year(apart) < 0)
            return APART_READ_ERR;

        // OWNER_COUNT
        if (fscanf(f, "%d", p_sec_owner_count(apart)) != 1)
            return APART_READ_ERR;

        if (clear_buf(f) || sec_owner_count(apart) < 0)
            return APART_READ_ERR;

        // WERE_THERE_ANIMALS
        if (string_read(tmp, STR_SIZE + 1, f))
            return APART_READ_ERR;

        if (strcmp(tmp, "yes") == 0)
            *(p_sec_were_there_animals(apart)) = true;
        else if (strcmp(tmp, "no") == 0)
            *(p_sec_were_there_animals(apart)) = false;
        else
            return APART_READ_ERR;
    }
    else
        return APART_READ_ERR;

    return APART_READ_OK;
}

void aparts_output(apart_t *aparts, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("\nIndex: %zu", i + 1);

        apart_output(&aparts[i]);
    }
}

int aparts_add(apart_t **aparts, size_t *size, size_t i)
{
    apart_t *tmp = NULL;
    apart_t new_apart;

    int code = 0;

    if ((code = apart_fill(&new_apart)))
    {   if (code == APART_FILL_ERR_BUF)
            return APART_ADD_ERR_INVALID_DATA_BUF;
        else
            return APART_ADD_ERR_INVALID_DATA_NO_BUF;
    }

    if ((tmp = malloc((*size + 1) * sizeof(apart_t))) == NULL)
        return APART_ADD_ERR_DATA_ALLOC;

    if (*aparts != NULL)
    {
        aparts_ncpy(tmp, *aparts, i - 1);
        aparts_ncpy(tmp + i, *aparts + i - 1, *size - (i - 1));

        free(*aparts);
        *aparts = NULL;
    }

    *size += 1;

    tmp[i - 1] = new_apart;

    *aparts = tmp;

    return APART_ADD_OK;
}

int aparts_delete(apart_t **aparts, size_t *size, size_t i)
{
    apart_t *tmp = NULL;

    if (*aparts == NULL)
        return APART_DELETE_ERR_NO_DATA;

    if (*size == 1)
    {
        free(*aparts);
        *aparts = NULL;

        *size = 0;

        return APART_DELETE_OK;
    }
    
    for (size_t z = i; z < *size - 1; z++)
        (*aparts)[z] = (*aparts)[z + 1];

    if ((tmp = malloc((*size) * sizeof(apart_t))) == NULL)
        return APART_DELETE_ERR_DATA_ALLOC;

    aparts_ncpy(tmp, *aparts, *size - 1);

    free(*aparts);
    *aparts = NULL;

    (*size)--;

    *aparts = tmp;

    return APART_DELETE_OK;
}

void aparts_ncpy(apart_t *dst, apart_t *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
        dst[i] = src[i];
}

int aparts_save(apart_t *aparts, size_t size)
{
    FILE *f = NULL;
    char filename[STR_SIZE + 1] = "";

    if (size == 0)
        return APART_WRITE_ERR_NO_DATA;

    if (string_read(filename, STR_SIZE + 1, stdin))
        return APART_WRITE_ERR_INVALID_FILENAME;

    if ((f = fopen(filename, "w")) == NULL)
        return APART_WRITE_ERR_FILE_OPEN;

    aparts_write(f, aparts, size);

    fclose(f);

    return APART_WRITE_OK;
}

void aparts_write(FILE *f, apart_t *aparts, size_t size)
{
    for (size_t i = 0; i < size; i++)
        apart_write(f, &aparts[i]);
}

int insertion_sort(void *base, size_t num, size_t size, int (*compare)(const void *, const void *, void *(*)(apart_t *)), void *(*key)(apart_t *))
{
    void *tmp = NULL;
    void *ielem = NULL;

    if ((ielem = malloc(size)) == NULL)
        return APART_SORT_ERR_ALLOC;

    for (size_t i = 1; i < num; i++)
    {
        tmp = shift_inc(base, i * size);
        assign(tmp, ielem, size);

        while (shift_dec(tmp, size) >= base && compare(shift_dec(tmp, size), ielem, key) > 0)
        {
            assign(shift_dec(tmp, size), tmp, size);

            tmp = shift_dec(tmp, size);
        }

        assign(ielem, tmp, size);
    }

    free(ielem);

    return APART_SORT_OK;
}

int gnome_sort(void *base, size_t num, size_t size, int (*compare)(const void *, const void *, void *(*)(apart_t *)), void *(*key)(apart_t *))
{
    size_t i = size;

    while (i < num * size)
    {
        if (compare(shift_inc(base, i - size), shift_inc(base, i), key) > 0)
        {
            swap(shift_inc(base, i - size), shift_inc(base, i), size);
            i -= size;

            if (i == 0)
                i += size;
        }
        else
            i += size;
    }

    return APART_SORT_OK;
}

void *shift_inc(void *p, size_t shift)
{
    char *tmp = p;

    return (void *)(tmp + shift);
}

void *shift_dec(void *p, size_t shift)
{
    char *tmp = p;

    return (void *)(tmp - shift);
}

void assign(void *src, void *dst, size_t size)
{
    char *p_dst = dst, *p_src = src;

    for (size_t i = 0; i < size; i++)
    {
        *p_dst = *p_src;
        
        p_dst++;
        p_src++;
    }
}

void swap(void *src, void *dst, size_t size)
{
    char *p_dst = dst, *p_src = src;
    char tmp;

    for (size_t i = 0; i < size; i++)
    {
        tmp = *p_src;
        *p_src = *p_dst;
        *p_dst = tmp;
        
        p_dst++;
        p_src++;
    }
}

int aparts_strcmp(const void *p1, const void *p2, void *(*param) (apart_t *))
{
    char *s1 = param((apart_t *)p1);
    char *s2 = param((apart_t *)p2);

    if (strcmp(s1, s2) > 0)
        return 1;
    else if (strcmp(s1, s2) == 0)
        return 0;
    else
        return -1;
}

int aparts_doublecmp(const void *p1, const void *p2, void *(*param) (apart_t *))
{
    double *d1 = param((apart_t *)p1);
    double *d2 = param((apart_t *)p2);

    if (*d1 - *d2 > EPS)
        return 1;
    else if (fabs(*d1 - *d2) < EPS)
        return 0;
    else
        return -1;
}

int aparts_intcmp(const void *p1, const void *p2, void *(*param) (apart_t *))
{
    int *i1 = param((apart_t *)p1);
    int *i2 = param((apart_t *)p2);

    if (*i1 > *i2)
        return 1;
    else if (*i1 == *i2)
        return 0;
    else
        return -1;
}

int aparts_output_by_price(apart_t *aparts, size_t size, double min_price, double max_price)
{
    bool it_has_sec = false;
    bool it_has_data = false;

    double price = 0.0;

    for (size_t i = 0; i < size; i++)
        if (is_primary(&aparts[i]) == false)
        {
            it_has_sec = true;

            price = square(&aparts[i]) * quad_meter_cost(&aparts[i]);

            if (price - min_price > -EPS && price - max_price < EPS && ! sec_were_there_animals(&aparts[i]) && room_count(&aparts[i]) == 2)
            {
                it_has_data = true;

                printf("Index: %zu", i + 1);
                apart_output(&aparts[i]);
            }
        }
    
    if (it_has_sec == false)
        return APART_BY_PRICE_ERR_NO_SECONDARY;
    else if (it_has_data == false)
        return APART_BY_PRICE_ERR_NO_DATA;

    return APART_BY_PRICE_OK;
}

// KEY

int statistics_get(apart_t *aparts, size_t size, keystat_t *keys)
{
    apart_t *aparts_cpy = NULL;
    keystat_t *keys_cpy = NULL;

    struct timespec t_beg, t_end;

    long time_insertion_aparts = 0;
    long time_gnome_aparts = 0;
    long time_insertion_keys = 0;
    long time_gnome_keys = 0;

    if ((aparts_cpy = malloc(size * sizeof(apart_t))) == NULL)
        return APART_STAT_ERR_ALLOC;
    
    if ((keys_cpy = malloc(size * sizeof(keystat_t))) == NULL)
    {
        free(aparts_cpy);

        return APART_STAT_ERR_ALLOC;
    }

    // APARTS TIME

    for (size_t i = 0; i < ITER_COUNT; i++)
    {
        aparts_ncpy(aparts_cpy, aparts, size);

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        if (insertion_sort(aparts_cpy, size, sizeof(apart_t), aparts_intcmp, (void *(*)(apart_t *))p_room_count))
        {
            free(aparts_cpy);
            free(keys_cpy);

            return APART_STAT_ERR_ALLOC;
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        time_insertion_aparts += 1000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec) / 1000;

        aparts_ncpy(aparts_cpy, aparts, size);

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        if (gnome_sort(aparts_cpy, size, sizeof(apart_t), aparts_intcmp, (void *(*)(apart_t *))p_room_count))
        {
            free(aparts_cpy);
            free(keys_cpy);

            return APART_STAT_ERR_ALLOC;
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        time_gnome_aparts += 1000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec) / 1000;

        // APARTS TIME

        // KEYS TIME

        keys_ncpy(keys_cpy, keys, size);

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        if (key_insertion_sort(keys_cpy, size, sizeof(keystat_t), keys_intcmp, (void *(*)(keystat_t *))key_p_room_count))
        {
            free(aparts_cpy);
            free(keys_cpy);

            return APART_STAT_ERR_ALLOC;
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        time_insertion_keys += 1000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec) / 1000;

        keys_ncpy(keys_cpy, keys, size);

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_beg);
        if (key_gnome_sort(keys_cpy, size, sizeof(keystat_t), keys_intcmp, (void *(*)(keystat_t *))key_p_room_count))
        {
            free(aparts_cpy);
            free(keys_cpy);

            return APART_STAT_ERR_ALLOC;
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        time_gnome_keys += 1000000 * (t_end.tv_sec - t_beg.tv_sec) + (t_end.tv_nsec - t_beg.tv_nsec) / 1000;
    }

    // KEYS TIME

    time_insertion_aparts /= ITER_COUNT;
    time_insertion_keys /= ITER_COUNT;
    time_gnome_aparts /= ITER_COUNT;
    time_gnome_keys /= ITER_COUNT;

    keys_ncpy(keys, keys_cpy, size);

    free(aparts_cpy);
    free(keys_cpy);

    stat_output(time_insertion_aparts, time_gnome_aparts, time_insertion_keys, time_gnome_keys, size);

    return APART_STAT_OK;
}

void aparts_output_by_keys(apart_t *aparts, size_t size, keystat_t *keys)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("\nIndex: %zu", keys[i].index);
        apart_output(&aparts[keys[i].index - 1]);
    }
}

void stat_output(long time_insertion_aparts, long time_gnome_aparts, long time_insertion_keys, long time_gnome_keys, size_t size)
{
    printf("\nTIME DATA (in microseconds):\n");
    printf("Time insertion sort (aparts): %ld\n", time_insertion_aparts);
    printf("Time insertion sort (keys): %ld\n", time_insertion_keys);
    printf("Time gnome sort (aparts): %ld\n", time_gnome_aparts);
    printf("Time gnome sort (keys): %ld\n", time_gnome_keys);

    printf("\nSORTS COMPARING (using aparts):\n");
    if (time_insertion_aparts < time_gnome_aparts)
        printf("Insertion sort is %.6lf%% faster, than Gnome sort\n", (((double)time_gnome_aparts / (double)time_insertion_aparts) - 1.0) * 100.0);
    else if (time_insertion_aparts > time_gnome_aparts)
        printf("Gnome sort is %.6lf%% faster, than Insertion sort\n", (((double)time_insertion_aparts / (double)time_gnome_aparts) - 1.0) * 100.0);
    else
        printf("Gnome sort and Insertion sort are equal\n");

    printf("\nSORTS COMPARING (using keys):\n");
    if (time_insertion_keys < time_gnome_keys)
        printf("Insertion sort is %.6lf%% faster, than Gnome sort\n", (((double)time_gnome_keys / (double)time_insertion_keys) - 1.0) * 100.0);
    else if (time_insertion_keys > time_gnome_keys)
        printf("Gnome sort is %.6lf%% faster, than Insertion sort\n", (((double)time_insertion_keys / (double)time_gnome_keys) - 1.0) * 100.0);
    else
        printf("Gnome sort and Insertion sort are equal\n");

    printf("\nINSERTION SORT COMPARING (using aparts and keys):\n");
    if (time_insertion_keys < time_insertion_aparts)
        printf("Insertion sort is %.6lf%% faster, if we using keys\n", (((double)time_insertion_aparts / (double)time_insertion_keys) - 1.0) * 100.0);
    else if (time_insertion_keys > time_insertion_aparts)
        printf("Insertion sort is %.6lf%% faster, if we using aparts\n", (((double)time_insertion_keys / (double)time_insertion_aparts) - 1.0) * 100.0);
    else
        printf("No matter, keys or aparts. Insertion sort has the same result\n");

    printf("\nGNOME SORT COMPARING (using aparts and keys):\n");
    if (time_gnome_keys < time_gnome_aparts)
        printf("Gnome sort is %.6lf%% faster, if we using keys\n", (((double)time_gnome_aparts / (double)time_gnome_keys) - 1.0) * 100.0);
    else if (time_gnome_keys > time_gnome_aparts)
        printf("Gnome sort is %.6lf%% faster, if we using aparts\n", (((double)time_gnome_keys / (double)time_gnome_aparts) - 1.0) * 100.0);
    else
        printf("No matter, keys or aparts. Gnome sort has the same result\n");

    printf("\nMEMORY (Insertion sort)\n");
    printf("MEMORY Insertion sort (aparts): %zu\n", ((size + 1) * sizeof(apart_t)));
    printf("MEMORY Insertion sort (keys): %zu\n", (size * (sizeof(apart_t) + sizeof(keystat_t)) + sizeof(keystat_t)));
    printf("\nMEMORY (Gnome sort)\n");
    printf("MEMORY Gnome sort (aparts): %zu\n", ((size * sizeof(apart_t))));
    printf("MEMORY Gnome sort (keys): %zu\n\n", (size * (sizeof(apart_t) + sizeof(keystat_t))));

    if ((size * (sizeof(apart_t) + sizeof(keystat_t)) + sizeof(keystat_t)) > ((size + 1) * sizeof(apart_t)))
        printf("Keys requires %.6lf%% bigger memory, than aparts sort\n", ((double)(size * (sizeof(apart_t) + sizeof(keystat_t)) + sizeof(keystat_t)) / (double)((size + 1) * sizeof(apart_t)) - 1.0) * 100.0);
    else if ((size * (sizeof(apart_t) + sizeof(keystat_t)) + sizeof(keystat_t)) < ((size + 1) * sizeof(apart_t)))
        printf("Aparts requires %.6lf%% bigger memory, than keys sort\n", ((double)((size + 1) * sizeof(apart_t)) / (double)(size * (sizeof(apart_t) + sizeof(keystat_t)) + sizeof(keystat_t)) - 1.0) * 100.0);
    else
        printf("Keys memory and aparts memory are equal\n");

    printf("\nMEMORY (Gnome sort)\n");
    printf("Keys requires %.6lf%% bigger memory, than aparts sort\n", ((double)(size * (sizeof(apart_t) + sizeof(keystat_t))) / (double)(size * sizeof(apart_t)) - 1.0) * 100.0);
}

int keys_create(apart_t *aparts, size_t size, keystat_t **keys)
{
    if (*keys != NULL)
    {
        free(*keys);
        *keys = NULL;
    }

    if ((*keys = malloc(size * sizeof(keystat_t))) == NULL)
        return APART_STAT_ERR_ALLOC;

    for (size_t i = 0; i < size; i++)
        key_fill(&(*keys)[i], room_count(&aparts[i]), i + 1);

    return APART_STAT_OK;
}

void key_fill(keystat_t *key, int room_count, size_t index)
{
    key->room_count = room_count;
    key->index = index;
}

int key_insertion_sort(void *base, size_t num, size_t size, int (*compare)(const void *, const void *, void *(*)(keystat_t *)), void *(*key)(keystat_t *))
{
    void *tmp = NULL;
    void *ielem = NULL;

    if ((ielem = malloc(size)) == NULL)
        return APART_SORT_ERR_ALLOC;

    for (size_t i = 1; i < num; i++)
    {
        tmp = shift_inc(base, i * size);
        assign(tmp, ielem, size);

        while (shift_dec(tmp, size) >= base && compare(shift_dec(tmp, size), ielem, key) > 0)
        {
            assign(shift_dec(tmp, size), tmp, size);

            tmp = shift_dec(tmp, size);
        }

        assign(ielem, tmp, size);
    }

    free(ielem);

    return APART_SORT_OK;
}

int key_gnome_sort(void *base, size_t num, size_t size, int (*compare)(const void *, const void *, void *(*)(keystat_t *)), void *(*key)(keystat_t *))
{
    size_t i = size;

    while (i < num * size)
    {
        if (compare(shift_inc(base, i - size), shift_inc(base, i), key) > 0)
        {
            swap(shift_inc(base, i - size), shift_inc(base, i), size);
            i -= size;

            if (i == 0)
                i += size;
        }
        else
            i += size;
    }

    return APART_SORT_OK;
}

int keys_intcmp(const void *p1, const void *p2, void *(*param) (keystat_t *))
{
    int *i1 = param((keystat_t *)p1);
    int *i2 = param((keystat_t *)p2);

    if (*i1 > *i2)
        return 1;
    else if (*i1 == *i2)
        return 0;
    else
        return -1;
}

void keys_ncpy(keystat_t *dst, keystat_t *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
        dst[i] = src[i];
}

void output_keys(keystat_t *keys, size_t size)
{
    printf("\nINDEX | KEY (room_count)\n");
    for (size_t i = 0; i < size; i++)
    {
        printf("%zu        %d\n", keys[i].index, keys[i].room_count);
    }
}

// KEY
