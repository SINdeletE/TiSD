#!/bin/bash

compare_pos(){
    if bash pos_case.sh "$1" "$2"; then
        echo OK
    else
        echo NOT OK
        exit 2
    fi
}

compare_neg(){
    if bash neg_case.sh "$1"; then
        echo OK
    else
        echo NOT OK
        exit 2
    fi
}

i=0
pos_tests="../data/pos_??_in.txt"
echo "POS TESTS"
for file_in in $pos_tests; do
    num=$(echo "$file_in" | grep -o "[0-9]*")

    if [[ -z "$num" ]]; then
        break
    fi

    i=$((i+1))
    echo "$i". "$(compare_pos "$file_in" ../data/pos_"$num"_out.txt)"
done

i=0
neg_tests="../data/neg_??_in.txt"
echo "NEG TESTS"
for file_in in $neg_tests; do
    num=$(echo "$file_in" | grep -o "[0-9]*")

    if [[ -z "$num" ]]; then
        break
    fi

    i=$((i+1))
    echo "$i". "$(compare_neg "$file_in")"
done

exit 0
