#!/bin/bash

graph() {

	PREFIX=$1
	strings=""
	echo $PREFIX
	:> trace
	for ff in ${PREFIX}*.tr;
	do
		FF0=${ff#$PREFIX}
		FF1=${FF0%.tr}
		FF2=${FF1//_/,}
		echo $FF2
		strings="$strings $FF2"
		grep accuracy $ff | sed "s/accuracy/$FF2/" | sort > temp1
		paste trace temp1 > temp2
		mv temp2 trace
	done

	sort -r -gk 5 trace > temp1
	mv temp1 trace

	title=${PREFIX//_/ }
	cat gn_one.plt | sed "s/STRINGS/$strings/" | \
	sed "s/TITLE/$title/" > my.plt 
	gnuplot  my.plt

	mv temp.eps s_$PREFIX.eps 
}

graph test_nodelay_2Mb_
graph test_delay_2Mb_
graph test_delayvar_2Mb_

gs -sDEVICE=pdfwrite -dNOPAUSE -dBATCH -dSAFER -sOutputFile=results.pdf s_*.eps

