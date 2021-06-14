#!/bin/bash

BINSIZE_MS=1000
NUM_BINS=10

# Ground Truth Label 0
./udprate ../QUIC_traces/QUIC-google-meet.pcapng $BINSIZE_MS $NUM_BINS 200 1000 0 > GT0.csv


# Ground Truth Label 1
./udprate ../QUIC_traces/RTP-google-meet.pcapng $BINSIZE_MS $NUM_BINS 326 -1 1 > GT1.csv

