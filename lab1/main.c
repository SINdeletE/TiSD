#include <stdio.h>
#include <stdlib.h>

#include "num_tools.h"
#include "string_tools.h"

#define IO_OK 0
#define IO_ERR 1

int main(void)
{
    int flag = 0;

    char double_num[STR_LEN] = "";
    char int_num[STR_LEN] = "";

    real_t real, res;
    inum_t inum;

    // printf("---------1---------2---------3---------4\n");

    if ((flag = input_str(double_num, STR_LEN)))
    {
        if (flag == OVERFLOW_BUFFER)
        {
            printf("OVERFLOW\n");

            return IO_ERR;
        }
        else if (flag == EMPTY_STRING)
        {
            printf("EMPTY STRING\n");

            return IO_ERR;
        }
        else
        {
            printf("ERROR\n");

            return IO_ERR;
        }
    }

    if (input_str(int_num, STR_LEN))
    {
        if (flag == OVERFLOW_BUFFER)
        {
            printf("OVERFLOW\n");

            return IO_ERR;
        }
        else if (flag == EMPTY_STRING)
        {
            printf("EMPTY STRING\n");

            return IO_ERR;
        }
        else
        {
            printf("ERROR\n");

            return IO_ERR;
        }
    }

    // We're checking correct form of num
    if ((flag = convert_to_real(double_num, &real)))
    {
        printf("INVALID EXP FORM\n");

        return IO_ERR;
    }

    if ((flag = convert_to_inum(int_num, &inum)))
    {
        printf("INVALID NUM FORM\n");

        return IO_ERR;
    }

    calculate(&res, &real, &inum);

    if (abs(res.order) < MAX_EXP)
        output(&res);
    else
    {
        printf("ERROR ORDER\n");
        
        return IO_ERR;
    }

    return IO_OK;
}
