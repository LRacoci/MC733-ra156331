#!/bin/bash

ex2=$(pwd)

#ls /opt/mc723/benchmarks/traces/

cd /opt/mc723/benchmarks/traces/

num=176
for nome in gcc mesa galgel art 
do
	cd $num.$nome.f2b/

	for size in 2 4 8 16 32 64 
	do 
		$ex2/dinero/d4-7/dineroIV \
		-informat s -trname $nome"_f2b" \
		-maxtrace 100 \
		-l1-isize $size"k" -l1-dsize $size"k" \
		-l1-ibsize 4 -l1-dbsize 4 \
		> $ex2/$nome"_"$size".out"
	done


	cd ..
	let num+=1
done



