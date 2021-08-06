#!/bin/bash

NETEM_CONF_DIR=/root/workspace/SatnexV_WP21/sat_emu_ns3/
OUTPUT_DIR=/root/workspace/SatnexV_WP21/QUIC_traces/

LOCAL_TUN=tun0

run_trace() {
    echo "RTT=$1 RTTVAR=$2 BANDWIDTH=$3 QUEUESIZE=$4"
	$NETEM_CONF_DIR/netem_config.sh $1 $2 $3 $4
	FILE_DUMP=$OUTPUT_DIR/youtube_$1_$2_$3_$4.pcap 
	tcpdump -U -s 64 -i $LOCAL_TUN -w $FILE_DUMP udp &
	TD_PID=$!
	sleep 10
	kill -9 $TD_PID
	rm -f $FILE_DUMP.gz
	gzip $FILE_DUMP
}


run_trace  10ms   1ms 2Mbit 50
run_trace 200ms   1ms 2Mbit 50
run_trace  50ms  50ms 2Mbit 50
run_trace  10ms   1ms 5Mbit 50
run_trace 200ms   1ms 5Mbit 50
run_trace  50ms  50ms 5Mbit 50

