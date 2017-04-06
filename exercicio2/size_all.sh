#!/bin/bash

ex2=$(pwd)

#ls /opt/mc723/benchmarks/traces/

#cd /opt/mc723/benchmarks/traces/
cd /home/ubuntu/benchmarks/traces
num=176

mkdir ${ex2}/size_all
mkdir ${ex2}/size_all/max$1

for nome in gcc mesa galgel art
do
	cd $num.$nome.f2b/
	this=${ex2}/size_all/max$1/${nome}
	mkdir ${this}

	for i in 2 4 8 16 32 64 128 256 512;
	do
		for d in 2 4 8 16 32 64 128 256 512;
		do
			echo "dineroIV -informat s -trname ${nome}_f2b -maxtrace $1 -l1-isize ${size}k -l1-dsize $sizek -l1-ibsize 4 -l1-dbsize 4 > $ex2/size_all/${nome}/${i}ki${d}kd.out"
			dineroIV \
				-informat s -trname ${nome}_f2b \
				-maxtrace $1 \
				-l1-isize ${i}k -l1-dsize ${d}k \
				-l1-ibsize 4 -l1-dbsize 4 \
				> ${this}/${i}ki${d}kd.out
		done
	done

	cd ..
	let num+=1
done
