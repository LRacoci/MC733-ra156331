#!/bin/bash

ex2=$(pwd)

#ls /opt/mc723/benchmarks/traces/

nome=( 'gcc' 'mesa' 'galgel' 'art' )
size=( 64 32 8 4)
traces=/opt/mc723/benchmarks/traces/
maxtraces=$1
parse=$ex2/others/parse.out

echo "d = {" > ${parse}
for((i=0;i<${#nome[@]};i++));
do
	this=$ex2/others/max${maxtraces}/${nome[$i]}_${size[$i]}
	echo "'${nome[$i]}' : np.array([" >>  ${parse}
	for bsize in 4 8 16 32 64 ; do 
		echo "[" >>  ${parse}
		for assoc in 1 2 4 8 16; do
			echo "[" >>  ${parse}
			for repl in l r f; do
			echo "[ #nums" >> ${parse}
			
			cat $this/bsize-${bsize}assoc-${assoc}repl-${repl} | grep "Demand miss rate" | sed 's/.*rate\s*\([0-9.]*\).*/\1/' >> ${parse}
			echo "]," >> ${parse}
			
			done
			echo "]," >>  ${parse}
		done
		echo "]," >>  ${parse}
	done

	echo "], dtype = np.float64)," >>  ${parse}

done

echo "}" >> ${parse}



