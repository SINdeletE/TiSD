#!/bin/bash
 
gcc -std=c99 -Wall -Werror -Wpedantic -Wextra --coverage -O0 -Wvla -g \
 -Wfloat-conversion -Wfloat-equal \
 -c ./*.c \
 -lm
 
gcc -o "app.exe" ./*.o -lgcov -lm
