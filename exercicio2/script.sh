#!/bin/bash

ex2=$(pwd)

cd /opt/mc723/benchmarks/traces/183.equake.f2b/

$ex2/dinero/d4-7/dineroIV \ 
	-informat s \ 
	-trname equake_f2b \ 
	-maxtrace 100 \ 
	-l1-isize 16k \ 
	-l1-dsize 16k \ 
	-l1-ibsize 16 \ 
	-l1-dbsize 16 > $ex2/out.txt

cd ex2

