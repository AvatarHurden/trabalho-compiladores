#!/bin/bash -e

for f in ./e3tests/*; do
    printf "\n\n\e[32mTEST $(basename $f)\e[0m\n"
    cat $f
    valgrind --leak-check=full --error-exitcode=1 ./etapa3 < $f
    ./etapa3 < $f > ./parse1
    ./etapa3 < parse1 > ./parse2
    diff parse1 parse2
    echo $?
done
