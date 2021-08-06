#!/bin/bash



run_trace() {
	/root/workspace/SatnexV_WP21/sat_emu_ns3/netem_config.sh $1 $2 $3 $4
	FILE_DUMP=youtube_$1_$2_$3_$4.pcap 
	tcpdump -U -s 64 -i utun2 -w $FILE_DUMP udp &
	TD_PID=$!
	sleep 3600
	kill -9 $TD_PID
	rm $FILE_DUMP.gz
	gzip $FILE_DUMP
}


run_trace  10ms   1ms 2Mbit 50
run_trace 200ms   1ms 2Mbit 50
run_trace  50ms  50ms 2Mbit 50
run_trace  10ms   1ms 5Mbit 50
run_trace 200ms   1ms 5Mbit 50
run_trace  50ms  50ms 5Mbit 50

