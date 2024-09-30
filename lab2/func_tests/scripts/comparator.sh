#!/bin/bash

if diff -w "$1" "$2"; then
    exit 0
else
    exit 2
fi
