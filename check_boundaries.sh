#!/bin/bash

FILE=${1:?"$0 <filename.pcap>"}

./udprate $FILE 1000 1 0 -1 0 > out.log
  
echo "plot 'out.log' u 0:1 w l" | gnuplot -p

