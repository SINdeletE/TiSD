#!/bin/bash

bash ./clean.sh

echo "BUILD DEBUG"
bash ./build_debug.sh
cd ./func_tests/scripts || exit 1
bash ./func_tests.sh
cd ../.. || exit 1

rm ./*.gcda

echo "BUILD DEBUG ASAN"
bash ./build_debug_asan.sh
cd ./func_tests/scripts || exit 1
bash ./func_tests.sh
cd ../.. || exit 1

rm ./*.gcda

echo "BUILD DEBUG MSAN"
bash ./build_debug_msan.sh
cd ./func_tests/scripts || exit 1
bash ./func_tests.sh
cd ../.. || exit 1

echo "BUILD DEBUG UBSAN"
bash ./build_debug_ubsan.sh
cd ./func_tests/scripts || exit 1
bash ./func_tests.sh
cd ../.. || exit 1
