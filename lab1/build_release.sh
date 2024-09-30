#!/bin/bash

gcc -std=c99 -Wall -Werror -Wpedantic -Wextra -Wvla -O2 -c ./*.c -lm

gcc -o "app.exe" ./*.o -lm
