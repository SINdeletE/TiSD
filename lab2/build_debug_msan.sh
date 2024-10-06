#!/bin/bash
 
clang -std=c99 -Wall -Werror -Wpedantic -Wextra -O0 -Wvla -g \
 -Wfloat-conversion -Wfloat-equal \
 -fsanitize=memory -fno-omit-frame-pointer -fPIE \
 -c *.c \
 
clang -o "app.exe" *.o -fsanitize=memory -pie
