#!/bin/bash -e

for f in ./e3tests/*; do
    printf "\n\n\e[32mTEST $(basename $f)\e[0m\n"
    cat $f
    valgrind --leak-check=full --error-exitcode=1 ./etapa2 < $f
    ./etapa2 < $f > ./parse1
    ./etapa2 < parse1 > ./parse2
    diff parse1 parse2
    echo $?
done
