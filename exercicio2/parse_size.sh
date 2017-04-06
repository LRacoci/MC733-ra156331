#!/bin/bash

ex2=$(pwd)

#ls /opt/mc723/benchmarks/traces/


num=176

parse=$ex2/size/parse.out
echo "d = {" > ${parse}
for nome in gcc mesa galgel art;
do
	echo "'${nome}' : np.array([" >>  ${parse}
	for size in 2 4 8 16 32 64;
	do 
		echo "[ #nums" >> ${parse}
		cat $ex2/size/$nome"_"$size".out"| grep "Demand miss rate" | sed 's/.*rate\s*\([0-9.]*\).*/\1/' >> ${parse}
		echo "]," >> ${parse}
	done
	echo "])," >>  ${parse}
	let num+=1
done
echo "}" >> ${parse}



