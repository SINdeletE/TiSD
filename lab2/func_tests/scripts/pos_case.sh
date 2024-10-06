#!/bin/bash 

if ! ../../app.exe < "$1" > ./temper.txt; then
	exit 2
fi


if bash ./comparator.sh "$2" ./temper.txt; then
    exit 0
else
    exit 2
fi
