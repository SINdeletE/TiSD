#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "string_tools.h"

int my_pow(int a, int n);

int input_str(char *str, int max_str_len);
int trim_spaces(char *dst, const char *src);

int is_exp_form(const char *str);
int is_int_form(char *str);

int input_str(char *str, int max_str_len)
{
    if (fgets(str, max_str_len, stdin) != str)
        return ERROR_READ;

    if (str[strlen(str) - 1] != '\n')
        return OVERFLOW_BUFFER;

    if (strlen(str) == EMPTY_STRING)
        return EMPTY_STRING;

    str[strlen(str) - 1] = '\0';

    return OK;
}

int trim_spaces(char *dst, const char *src)
{
    size_t dst_idx = 0;
    int is_start = 0;
    int is_end = 0;
    int exit_code = 0;

    for (size_t i = 0; src[i] != '\0'; i++)
        if (!isspace(src[i]))
        {
            dst[dst_idx] = src[i];
            dst_idx++;
            is_start = 1;

            if (is_end)
                exit_code = 1;
        }
        else if (is_start)
            is_end = 1;

    dst[dst_idx] = '\0';

    return exit_code;
}

int is_exp_part(const char *part, int part_s, int is_two_parts)
{
    int is_start = 1;
    int exp_in = 0;

    if (strlen(part) == 0 && is_two_parts)
        return 1;

    while (*part)
    {
        if (strchr(SIGNS, *part) && ((is_start && part_s == 1) || strchr(EXP, part[-1])))
            part++;
        else if (strchr(SIGNS, *part) && !exp_in)
            return 0;

        if (strchr(EXP, *part) && !exp_in && (!is_start || is_two_parts))
            exp_in = 1;
        else if (!isdigit(*part))
            return 0;

        part++;
        is_start = 0;
    }

    return isdigit(*(--part)) ? 1 : 0;
}

int is_exp_form(const char *str)
{
    char buff[STR_LEN] = "\0";
    char *second_part = strchr(str, DOT);

    int state = 0;
    int part = 2;
    int is_two_parts = 0;

    if (second_part != NULL)
    {
        strncpy(buff, str, (unsigned long)(second_part - str));
        buff[strlen(buff)] = '\0';

        if ((strlen(buff) == 1 && strchr(SIGNS, buff[0]) != NULL) || !strlen(buff))
            state = is_exp_part(second_part + 1, part, is_two_parts);
        else if (strcspn(buff, EXP) == strlen(buff))
            state = is_exp_part(buff, --part, is_two_parts) &&
                is_exp_part(second_part + 1, ++part, ++is_two_parts);
    }
    else
    {
        strcpy(buff, str);
        state = is_exp_part(buff, --part, is_two_parts);
    }

    return state;
}

int is_exp_valid(char *str, bool *sign, size_t *result_mantissa, int *result_order)
{
    int num_exp = 0;
    size_t fraction_size = 0;
    char *p = str, *p_left = NULL, *p_right = NULL;

    // helps to work with <1 fractions
    bool flag_neg_order = true;

    *sign = false;
    *result_order = 0;
    *result_mantissa = 0;

    if (*p == '+' || *p == '-')
    {
        if (*p == '-')
            *sign = true;

        p++;
    }

    while (*p == '0')
        p++;

    // if it has non-zero int part
    if (isdigit(*p))
        flag_neg_order = false;

    // int part of num
    while (isdigit(*p))
    {
        (*result_order)++;
        (*result_mantissa)++;

        p++;
    }
    
    // if num is only int
    if (*p == '\0')
        return abs(*result_order) < my_pow(DEC, MAX_ORDER_NUM_LEN) && *result_mantissa <= MAX_MAN_NUM_LEN;

    else if (*p == '.')
    {
        fraction_size = strspn(p + 1, DIGITS);

        if (fraction_size != 0)
        {
            // to the first digit after DOT
            p_left = p + 1;

            // to the last digit after DOT
            p_right = p + fraction_size;
            
            // skip all zeros in p_right
            while (*p_right == '0' && p_left < p_right)
                p_right--;

            // all num is ZERO (3-rd condition is good as fuck)
            if (flag_neg_order && p_left == p_right && *p_left == '0')
                return 1;

            while (*p_left == '0')
            {
                if (flag_neg_order)
                    (*result_order)--;
                else
                    (*result_mantissa)++;

                p_left++;
            }

            // *result_order += p_right - p_left + 1;
            *result_mantissa += p_right - p_left + 1;   
        }
    }

    p = strpbrk(p, EXP);
    
    if (p != NULL)
    {
        p++;

        // The number after exponential symbol
        num_exp = atoi(p);

        *result_order += num_exp;
    }

    return abs(*result_order) < my_pow(DEC, MAX_ORDER_NUM_LEN) && *result_mantissa <= MAX_MAN_NUM_LEN;
}

void mantissa_fill(char *str, real_t *real)
{
    char *p = str;
    char *p_left = NULL, *p_right = NULL;

    size_t s = 0;

    bool flag_neg_order = true;

    if (strchr(SIGNS, *p))
        p++;

    while (*p == '0')
        p++;

    while (isdigit(*p))
    {
        flag_neg_order = false;

        real->mantissa[real->mantissa_size - s - 1] = *p - ZERO_CONST;
        s++;

        p++;
    }

    if (*p == '.' && ! strchr(EXP, *(p + 1)))
    {
        p_right = p + 1;

        while (isdigit(*p_right))
            p_right++;

        while (*p_right == '0')
            p_right--;

        p_left = p + 1;

        while (flag_neg_order && *p_left == '0')
            p_left++;

        while (p_left < p_right)
        {
            flag_neg_order = false;

            real->mantissa[real->mantissa_size - s - 1] = *p_left - ZERO_CONST;
            s++;

            p_left++;
        }
    }

    if (flag_neg_order)
        real->mantissa_size = 1;
}

int convert_to_real(char *str, real_t *real)
{
    int flag;
    *real = (real_t) {false, 0, {0}, 0};

    if (! (flag = is_exp_form(str)))
        return ERROR_READ;

    if (! (flag = is_exp_valid(str, &real->sign, &real->mantissa_size, &real->order)))
        return ERROR;

    mantissa_fill(str, real);

    return OK;
}

int is_inum_form(char *str)
{
    size_t l = 0;

    if (*str == '+' || *str == '-')
        str++;

    while (*str == '0')
        str++;

    if (*str == '\0')
        return 1;

    while (str[l] != '\0')
    {
        if (! isdigit(str[l]))
            return 0;

        l++;
    }

    if (l > MAX_INT_NUM_LEN)
        return 0;

    return 1;
}

int convert_to_inum(char *str, inum_t *inum)
{
    int flag;
    size_t i = 0;

    char *p = str;

    inum->sign = false;
    inum->num_size = 0;

    if (! (flag = is_inum_form(str)))
        return ERROR;
    
    if (strchr(SIGNS, *p))
    {
        if (*p == '-')
            inum->sign = true;

        p++;
    }

    while (*p == '0')
        p++;

    if (*p == '\0')
    {
        inum->num[inum->num_size] = 0;
        inum->num_size++;

        return OK;
    }

    while (p[inum->num_size] != '\0')
        inum->num_size++;

    i = 0;

    while (p[i] != '\0')
    {
        inum->num[inum->num_size - i - 1] = p[i] - ZERO_CONST;

        i++;
    }

    return OK;
}

int my_pow(int a, int n)
{
    int num = 1;

    for (int i = 0; i < n; i++)
        num *= a;

    return num;
}
