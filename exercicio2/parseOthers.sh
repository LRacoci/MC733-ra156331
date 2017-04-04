#!/bin/bash

ex2=$(pwd)

#ls /opt/mc723/benchmarks/traces/

cd /opt/mc723/benchmarks/traces/

nome=( 'gcc' 'mesa' 'galgel' 'art' )
num=( '176' '177' '178' '179' )
size=( 64 32 8 4)
traces=/opt/mc723/benchmarks/traces/
maxtraces=$1
{parse}=$ex2/{parse}.out

cd ${traces}

echo "{" >> ${parse}
for((i=0;i<${#nome[@]};i++));
do
	this=$ex2/max${maxtraces}/${nome[$i]}_${size[$i]}

	echo "${nome[i]}:" >> ${parse}
	
	for bsize in 4 8 16 32 64 ;
	do 
		for assoc in 1 2 4 8 16;
        do
		
		echo "	(($bsize, $assoc), " >> ${parse}

		cat $this/bsize${bsize}assoc${assoc} | grep "Demand miss rate" >> ${parse}

		echo "	)" >> ${parse}
		done
	done

done

echo "}" >> ${parse}



