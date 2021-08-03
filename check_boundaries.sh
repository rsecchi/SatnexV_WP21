#!/bin/bash

FILE=${1:?"$0 <filename.pcap>"}

if [ ${FILE: -3} == ".gz" ]; then
	TMPFILE=$(mktemp -u)
	gzip -d $FILE  --stdout > $TMPFILE
	./udprate $TMPFILE 1000 1 0 -1 0 > out.log
	rm $TMPFILE
else
	./udprate $file 1000 1 0 -1 0 > out.log
fi

  
echo "plot 'out.log' u 0:1 w l" | gnuplot -p

