#!/usr/bin/gnuplot --persist 

set term postscript

#set size 1000,400
set xtics rotate by 45 right

set yrange [0.5:1.02]
set offset 0.5, 0.5 , 0, 0
set grid

set errorbars 4
set boxwidth 0.4

in_data="DATAFILE.tr"

in_file = "< grep accuracy ".in_data
set title 'accuracy,TITLE'
set out "g_acc_DATAFILE.eps"
plot in_file u 0:4:3:7:6:xticlabels(1) notitle with candlesticks whiskerbars 0.5

in_file = "< grep prec ".in_data
set title 'precision (labels), TITLE'
set out "g_prec_DATAFILE.eps"
plot in_file u 0:4:3:7:6:xticlabels(stringcolumn(1)."-".stringcolumn(2)) notitle with candlesticks whiskerbars 0.5 

in_file = "< grep [^p]rec ".in_data
set title 'sensitivity (labels), TITLE'
set out "g_sens_DATAFILE.eps"
plot in_file u 0:4:3:7:6:xticlabels(stringcolumn(1)."-".stringcolumn(2)) notitle with candlesticks whiskerbars 0.5 



