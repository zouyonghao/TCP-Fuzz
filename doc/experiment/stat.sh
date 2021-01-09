#! /bin/bash

for ((i = 0; i <= 24; ++i));
do
    # echo $i"h"
    # gawk -v pattern=$(echo $i"h") '$0 ~ pattern {print $5; exit}' $1
    gawk -v pattern=$(echo $i"h") '$0 ~ pattern {print $5; exit}' $1 | cut -d':' -f 2 | cut -d'(' -f 1
done
