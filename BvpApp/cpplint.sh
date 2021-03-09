#!/bin/bash

python cpplint.py --counting=detailed \
    $( find . -name \*.h -or -name \*.hpp -or -name \*.cpp -or -name \*.c | grep -vE "^\.\/build\/" ) 2>&1 | \
    grep -e "Category" -e "Total error"

#python cpplint.py --counting=detailed  $( find . -name \*.h -or -name \*.cpp | grep -vE "^\.\/build\/" ) 2>&1 |     grep -e "Category" -e "Total error"    