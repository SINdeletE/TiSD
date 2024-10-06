#!/bin/bash 

if ../../app.exe < "$1" > ./temper.txt; then
    exit 2
else
    exit 0
fi
