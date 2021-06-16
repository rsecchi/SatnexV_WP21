#!/bin/bash

BINSIZE_MS=1000
NUM_BINS=10


trap 'rm -f "$TMP0" "$TMP1" "$TMP2"' EXIT
TMP0=$(mktemp) || exit 1
TMP1=$(mktemp) || exit 1
TMP2=$(mktemp) || exit 1

cat filenames |\
while read fname start end label cmt
do

	if [ -z $fname ]; then continue; fi
	if [ $fname = "#" ]; then continue; fi

	echo ./udprate $fname $BINSIZE_MS $NUM_BINS $start $end $label 
	./udprate $fname $BINSIZE_MS $NUM_BINS $start $end $label > $TMP0

	awk 'NR==1' $TMP0 > $TMP1
	awk 'NR>1'  $TMP0 >> $TMP2
		
done

cat $TMP1 $TMP2 > GT.csv



