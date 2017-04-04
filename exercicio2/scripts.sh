#!/bin/bash

ex2=$(pwd)

#ls /opt/mc723/benchmarks/traces/

cd /opt/mc723/benchmarks/traces/

nome=( 'gcc' 'mesa' 'galgel' 'art' )
num=( '176' '177' '178' '179' )
size=( 64 32 8 4)
traces=/opt/mc723/benchmarks/traces/
maxtraces=$1


cd ${traces}

mkdir $ex2/others/max${maxtraces}
for((i=0;i<${#nome[@]};i++));
do
	cd ${num[$i]}.${nome[$i]}.f2b/
	mkdir $ex2/others/max${maxtraces}/${nome[$i]}_${size[$i]}

	this=$ex2/others/max${maxtraces}/${nome[$i]}_${size[$i]}

	for bsize in 4 8 16 32 64 ; do 
		for assoc in 1 2 4 8 16; do
			for repl in r f;	do
			echo "$$ex2/dinero/d4-7/dineroIV -informat s -trname ${nome[$i]}_f2b -maxtrace ${maxtraces} -l1-isize ${size[$i]}k -l1-dsize ${size[$i]}k -l1-ibsize ${bsize} -l1-dbsize ${bsize} -l1-iassoc ${assoc} -l1-dassoc ${assoc} -l1-irepl ${repl} -l1-drepl ${repl} > $this/bsize-${bsize}assoc-${assoc}repl-${repl}"
			
			$ex2/dinero/d4-7/dineroIV \
				-informat s -trname ${nome[$i]}_f2b \
				-maxtrace ${maxtraces} \
				-l1-isize ${size[$i]}k -l1-dsize ${size[$i]}k \
				-l1-ibsize ${bsize} -l1-dbsize ${bsize} \
				-l1-iassoc ${assoc} -l1-dassoc ${assoc} \
				-l1-irepl ${repl} -l1-drepl ${repl} \
				> $this/bsize-${bsize}assoc-${assoc}repl-${repl}
			
			done
		done
	done


	cd ..
	let num+=1
done



