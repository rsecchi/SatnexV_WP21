#!/usr/bin/gnuplot --persist 

set term postscript
set out "g_DATAFILE.eps"

#set size 1000,400
set xtics rotate by 45 right

set yrange [0.8:1.02]
set offset 0.5, 0.5 , 0, 0
set grid

set errorbars 4
set boxwidth 0.4

in_data="DATAFILE.tr"
in_data="test_nodelay_5Mb_minmax_100.tr" 

in_file = "< grep [^p]rec ".in_data
set title 'sensitiviy,TITLE'
plot in_file u 0:4:3:7:6:xticlabels(stringcolumn(1)."-".stringcolumn(2)) notitle with candlesticks whiskerbars 0.5

