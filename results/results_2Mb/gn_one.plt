#!/usr/bin/gnuplot --persist 

set term postscript

plot_list = "STRINGS"
string(n) = word(plot_list, n)

#set size ratio 0.3


set xtics rotate by 45 right

set yrange [0.6:1.03]
set offset 0.5, 0.5, 0, 0
set grid

set key bottom left 
set boxwidth 0.1


set title 'accuracy, TITLE'
set out "temp.eps"

plot "trace" u 0:4:3:7:6:xtic(1) t string(1) w cand lt 1 fill empty, \
     for [i=1:words(plot_list)-1] "trace" u ($0+i*0.15):(column(4+i*7)):(column(3+i*7)):(column(7+i*7)):(column(6+i*7)) \
		 t string(i+1) w cand lt 1 fill pattern i


