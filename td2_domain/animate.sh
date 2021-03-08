#!/bin/sh

make 1> /dev/null
COLS=512
HOP=32
i=0

mkdir img 2> /dev/null

while [ $i -lt $COLS ]
do
    ./scroll $i 0 ../data/earth.ppm a.ppm;    
    ./sphere a.ppm img/"a${i}".ppm;
    i=$(($i+$HOP));
done

convert -delay 20 -loop 0 `ls -v img/*` earth.gif

rm -rf img
