#!/usr/bin/gnuplot --persist 

set term postscript

#set size 1000,400
set xtics rotate by 45 right

set yrange [0.8:1.05]
set offset 0.5, 0.5 , 0, 0
set grid

set errorbars 4
set boxwidth 0.2

in_data="DATAFILE"

set title 'Normal. comparison (labels), TITLE'
set out "OUTFILE"
plot in_data u 0:4:3:7:6:xticl(1) every 2 title "minmax" w cand whiskerbars 0.5 fill solid lt 1, \
     in_data u ($0+0.2):4:3:7:6 every 2::1 title "stdnorm" w cand whiskerbars 0.5 fill empty lt 1



