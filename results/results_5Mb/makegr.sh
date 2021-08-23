#!/bin/bash

for ff in *tr;
do
	ffs=$(basename $ff .tr)
	title=$(echo ${ffs#test_} | tr '_' ',')
	sed "s/DATAFILE/$ffs/" gn.plt | sed "s/TITLE/$title/" | gnuplot
	echo $ff
done

gs -sDEVICE=pdfwrite -dNOPAUSE -dBATCH -dSAFER -sOutputFile=results.pdf *.eps

