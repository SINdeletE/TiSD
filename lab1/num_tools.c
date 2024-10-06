#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

#include "string_tools.h"
#include "num_tools.h"

size_t reg_rm_unsignif_zeros(reg_t reg, size_t *reg_size)
{
    size_t n_zero = 0;

    while (*reg_size > n_zero + 1 && reg[n_zero] == 0)
        n_zero++;
    
    // Убираем незначащие нули в мантиссе + увеличивает порядок
    for (size_t i = n_zero; i < *reg_size; i++)
        reg[i - n_zero] = reg[i];

    *reg_size -= n_zero;

    return n_zero;
}

bool reg_decs_move(reg_t reg, size_t reg_size)
{
    // Перенос десятков в соответствующие разряды
    for (size_t i = 0; i < reg_size - 1; i++)
        if (reg[i] >= DEC)
        {
            reg[i + 1] += reg[i] / DEC;
            reg[i] = reg[i] % DEC;
        }

    // Добавление нового десятка (если он имеется)
    if (reg[reg_size - 1] >= DEC)
    {
        reg[reg_size] += reg[reg_size - 1] / DEC;
        reg[reg_size - 1] = reg[reg_size - 1] % DEC;
        
        return true;
    }

    return false;
}

int reg_round(reg_t reg, size_t reg_size, size_t signif, size_t *rm)
{
    if (reg_size <= signif)
        return IS_NOT_ROUNDED;

    if (reg[reg_size - signif - 1] >= ROUND_CONST)
        reg[reg_size - signif]++;

    reg[reg_size - signif - 1] = 0;

    // двигаем signif чисел к началу
    for (size_t i = reg_size - signif; i < reg_size; i++)
    {
        reg[i - (reg_size - signif)] = reg[i];

        // Без этого действия reg_decs_move будет работать бесконечно от только что передвинутых чисел
        reg[i] = 0;
    }

    // Сколько чисел было опущено в ходе округления
    *rm += reg_size - signif;

    if (reg_decs_move(reg, signif))
        return IS_ROUNDED_AND_DEC_MOVE;

    return IS_ROUNDED;
}

void reg_to_real(reg_t reg, size_t reg_size, real_t *res)
{
    if (reg_size > SIGNIF_DIGITS)
    {
        for (size_t i = reg_size - SIGNIF_DIGITS; i < reg_size; i++)
            res->mantissa[reg_size - (i - (reg_size - SIGNIF_DIGITS)) - 1] = reg[i];

        res->mantissa_size = SIGNIF_DIGITS;
    }
    else
    {
        for (size_t i = 0; i < reg_size; i++)
            res->mantissa[reg_size - i - 1] = reg[i];

        res->mantissa_size = reg_size;
    }
}

void calculate(real_t *res, real_t *real, inum_t *inum)
{
    reg_t reg = {0};
    size_t reg_size = real->mantissa_size;
    size_t rm = 0;

    int flag = IS_NOT_ROUNDED;

    // RES Знак числа (xor)
    res->sign = (real->sign && ! inum->sign) || (! real->sign && inum->sign);

    // RES Переносим значение порядка
    res->order = real->order;

    // Умножает каждую цифру целого числа на цифры мантиссы
    for (size_t i = 0; i < inum->num_size; i++)
    {
        for (size_t z = 0; z < real->mantissa_size; z++)
            reg[i + z] += inum->num[i] * real->mantissa[z];

        // // Перенос десятков в соответствующие разряды
        // if (reg_decs_move(reg, reg_size) || inum->num[i] == 0)
        //     reg_size++;
    }

    // ПРИМЕРНОЕ КОЛ-ВО ЦИФР ПОСЛЕ УМНОЖЕНИЯ
    reg_size = real->mantissa_size - 1 + inum->num_size;

    // Перенос десятков в соответствующие разряды
    while (reg_decs_move(reg, reg_size))
        reg_size++;

    // Убирает незначащие нули и изменяет размер регистра + Увеличиваем порядок
    res->order += reg_rm_unsignif_zeros(reg, &reg_size);

    // Округляем число до SIGNIF_DIGITS значащих цифр
    while ((flag = reg_round(reg, reg_size, SIGNIF_DIGITS, &rm)) == IS_ROUNDED_AND_DEC_MOVE)
        reg_size = SIGNIF_DIGITS + 1; // КОНСТАНТА + 1 ДЛЯ СЛЕДУЮЩЕЙ ИТЕРАЦИИ

    // В таком случае дилна регистра будет КОНСТАНТОЙ
    if (flag == IS_ROUNDED)
        reg_size = SIGNIF_DIGITS;

    // Убирает незначащие нули и изменяет размер регистра + Увеличиваем порядок
    res->order += reg_rm_unsignif_zeros(reg, &reg_size);

    // Изменяем порядок разницей размеров регистра и мантиссы изначального ЧПТ
    if (reg_size > real->mantissa_size)
        res->order += reg_size - real->mantissa_size + rm;
    else
        res->order -= real->mantissa_size - reg_size - rm;

    // RES мантисса + размер мантиссы
    reg_to_real(reg, reg_size, res);
}

void output(real_t *real)
{
    if (real->sign)
        printf("-");

    printf("0.");

    for (size_t i = 0; i < real->mantissa_size; i++)
        printf("%d", real->mantissa[i]);

    if (! (real->mantissa_size == 1 && real->mantissa[0] == 0))
        printf("e%+d", real->order);

    printf("\n");
}
