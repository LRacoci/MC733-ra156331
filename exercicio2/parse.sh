#!/bin/bash

ex2=$(pwd)

#ls /opt/mc723/benchmarks/traces/

cd /opt/mc723/benchmarks/traces/

echo "" > $ex2/parse.out

for nome in gcc mesa galgel art 
do
	echo ${nome} >> $ex2/parse.out
	for size in 2 4 8 16 32 64 
	do 
		echo ${size} >> $ex2/parse.out
		cat $ex2/$nome"_"$size".out" | grep "l1-icache\|l1-dcache\|Demand miss rate" >> $ex2/parse.out
	done

done