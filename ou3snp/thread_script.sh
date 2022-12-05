#!/bin/bash
TIMEFORMAT='%3R'
for n in $(seq 1 100)
do
 echo "Running mdu on /pkg/ with $n threads."
 { time ./mdu /pkg/ -j $n 2>/dev/null 1>&2 ; } 2>> result5.txt
done
