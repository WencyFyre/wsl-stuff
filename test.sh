#!/bin/sh
#automated test for second bead

gcc bead.c storage.c vaccinate.c -o out.out
> storage.txt
./out.out --add --name "Vakci Gergő" --birth 1969 --tel 36301234567 --pay y
./out.out --add --name "Vakci Áci" --birth 1953 --tel 36301234567 --pay y
./out.out --add --name "Bálint Alma" --birth 1993 --tel 36332234587 --pay y
./out.out --add --name "Pista Krumpli" --birth 1913 --tel 36303134566 --pay y
./out.out --add --name "Hajdu Valaki" --birth 1983 --tel 36301234567 --pay n
./out.out --add --name "Acsi Áci" --birth 2015 --tel 36301233271 --pay n
./out.out --add --name "Antik Áci" --birth 2001 --tel 36332456733 --pay y
./out.out --add --name "Vakci áci" --birth 1987 --tel 36301234567 --pay n
./out.out --add --name "Vakci Gergő" --birth 1969 --tel 36301234567 --pay y
./out.out --add --name "Vakci Áci" --birth 1953 --tel 36301234567 --pay y
./out.out --add --name "Bálint Alma" --birth 1993 --tel 36332234587 --pay y
./out.out --add --name "Pista Krumpli" --birth 1913 --tel 36303134566 --pay y
./out.out --add --name "Hajdu Valaki" --birth 1983 --tel 36301234567 --pay n
./out.out --add --name "Acsi Áci" --birth 2015 --tel 36301233271 --pay n



./out.out --remove 7
./out.out --remove 8
./out.out --remove 11
./out.out --remove 12
./out.out --vaccinate
./out.out --list