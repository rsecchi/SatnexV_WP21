#!/bin/bash

BINSIZE_MS=1000
NUM_BINS=10

# Ground Truth Label 0
./udprate ../QUIC_traces/RTP-google-meet.pcapng $BINSIZE_MS $NUM_BINS 326 -1 0 > GT.csv

# Ground Truth Label 1
./udprate ../QUIC_traces/youtube+web.pcpng.pcapng $BINSIZE_MS $NUM_BINS 551 630 1 | \
 awk 'NR>1' >> GT.csv

