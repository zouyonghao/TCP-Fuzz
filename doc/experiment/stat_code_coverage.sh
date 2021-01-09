#! /bin/bash

for ((i = 0; i <= 48; ++i));
do
    # echo $i"h"
    # gawk -v pattern=$(echo $i"h") '$0 ~ pattern {print $5; exit}' $1
    gawk -v pattern=$(echo $i"h") '$0 ~ pattern {print $2; exit}' $1 | cut -d'(' -f 1
done
