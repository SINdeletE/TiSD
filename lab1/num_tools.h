#ifndef NUM_TOOLS_H__

#define NUM_TOOLS_H__

#include <stddef.h>
#include <stdbool.h>

#define MAX_INT_NUM_LEN 30

#define MAX_EXP 100000
#define MAX_MAN_NUM_LEN 40
#define MAX_ORDER_NUM_LEN 5
#define SIGNIF_DIGITS 30

#define REG_LEN (MAX_MAN_NUM_LEN + MAX_INT_NUM_LEN)

#define IS_NOT_ROUNDED 0
#define IS_ROUNDED 1
#define IS_ROUNDED_AND_DEC_MOVE 2

#define DEC 10
#define ROUND_CONST 5

typedef int reg_t[REG_LEN];

typedef struct
{
    bool sign;

    size_t mantissa_size;
    int mantissa[MAX_MAN_NUM_LEN + 1];
    
    int order;
} real_t;

typedef struct
{
    bool sign;
    
    size_t num_size;
    int num[MAX_INT_NUM_LEN + 1];
} inum_t;

void calculate(real_t *res, real_t *real, inum_t *inum);
void output(real_t *real);

#endif
