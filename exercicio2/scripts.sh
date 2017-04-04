#!/bin/bash

ex2=$(pwd)

#ls /opt/mc723/benchmarks/traces/

cd /opt/mc723/benchmarks/traces/

nome=( 'gcc' 'mesa' 'galgel' 'art' )
num=( '176' '177' '178' '179' )
size=( 64 32 8 4)


for((i=0;i<${#nome[@]};i++));
do
	cd ${num[$i]}.${nome[$i]}.f2b/
	mkdir $ex2/others/${nome[$i]}_${size[$i]}

	for bsize in 4 8 16 32 64 ;
	do 
		for assoc in 1 2 4 8 16;
		do
			$ex2/dinero/d4-7/dineroIV \
			-informat s -trname "${nome[$i]}_f2b" \
			-maxtrace 20 \
			-l1-isize "${size[$i]}k" -l1-dsize "${size[$i]}k" \
			-l1-ibsize ${bsize} -l1-dbsize ${bsize} \
			-l1-iassoc ${assoc} -l1-dassoc ${assoc}
			> $ex2/others/"${nome[$i]}_${size[$i]}"/"bsize${size[$i]}assoc${assoc}"
		done
	done


	cd ..
	let num+=1
done



