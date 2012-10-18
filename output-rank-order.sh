#! /bin/sh
for i in $(seq 0 99); do echo; echo $i; echo; grep "dest-rank $i " out ;done
