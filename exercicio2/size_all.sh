#!/bin/bash

ex2=$(pwd)

#ls /opt/mc723/benchmarks/traces/

#cd /opt/mc723/benchmarks/traces/
cd /home/ubuntu/benchmarks/traces
num=176

mkdir ${ex2}/size_all
for nome in gcc mesa galgel art
do
	cd $num.$nome.f2b/
	mkdir ${nome}

	for i in 2 4 8 16 32 64 128 256 512;
	do
		for d in 2 4 8 16 32 64 128 256 512;
		do
			$ex2/dinero/d4-7/dineroIV \
				-informat s -trname ${nome}_f2b \
				-maxtrace $1 \
				-l1-isize $sizek -l1-dsize $sizek \
				-l1-ibsize 4 -l1-dbsize 4 \
				> $ex2/size_all/${nome}/${i}ki${d}kd.out
		done
	done

	cd ..
	let num+=1
done
