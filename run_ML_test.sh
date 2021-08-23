#!/bin/bash

BINSIZE_MS=${1:-100}
NUM_BINS=${2:-100}
FILENAMES=${3:-filenames}

trap 'rm -f "$TMP0" "$TMP1" "$TMP2"' EXIT
TMP0=$(mktemp) || exit 1
TMP1=$(mktemp) || exit 1
TMP2=$(mktemp) || exit 1

cat $FILENAMES |\
while read fname start end label cmt
do

	if [ -z $fname ]; then continue; fi
	if [ $fname = "#" ]; then continue; fi
	if [ $fname = "@" ]; then
		eval $start $end $label $cmt
	fi

	echo $fname $start $end

	if [ ${fname: -3} == ".gz" ]; then
		TMPFILE=$(mktemp -u)
		trap 'rm -f "$TMPFILE"' EXIT
		gzip -d $fname --stdout > $TMPFILE
		fname=$TMPFILE
	fi

	./udprate $fname $BINSIZE_MS $NUM_BINS $start $end $label > $TMP0

	awk 'NR==1' $TMP0 > $TMP1
	awk 'NR>1'  $TMP0 >> $TMP2
		
done

echo cat $TMP1 $TMP2 \> GT${TAG}.csv > log
cat $TMP1 $TMP2 > GT${TAG}.csv 

./ml_engine.py

