#!/bin/bash

ex2=$(pwd)

#ls /opt/mc723/benchmarks/traces/
num=176

parse=$ex2/size_all/parse.out
echo "d = {" > ${parse}
for nome in gcc mesa galgel art
do
	this=${ex2}/size_all/max$1/${nome}
	echo "'${nome}' : np.array([" >>  ${parse}
	for i in 2 4 8 16 32 64 128 256 512;
	do
		echo "[" >>  ${parse}
		for d in 2 4 8 16 32 64 128 256 512;
		do
			echo "[ #nums" >> ${parse}
			cat ${this}/${i}ki${d}kd.out | grep "Demand miss rate" | sed 's/.*rate\s*\([0-9.]*\).*/\1/' >> ${parse}
			echo "]," >> ${parse}
		done
		echo "]," >>  ${parse}
	done
	echo "]," >>  ${parse}

	let num+=1
done
echo "}" >> ${parse}
