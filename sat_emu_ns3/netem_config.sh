#!/bin/bash

# Name on the local interface on public network
LOCAL_ETH=wlp2s0
LOCAL_TUN=tun0


DELAY=200ms         # average delay
DELAY_VAR=10ms      # uniform distribution
BANDWIDTH=2Mbit     # downlink bandwidth
QLEN=50             # buffer size in packets


#############################

ifconfig $LOCAL_TUN txqueuelen $QLEN

tc qdisc del dev $LOCAL_TUN root
tc qdisc add dev $LOCAL_TUN root tbf rate $BANDWIDTH  buffer 1600 limit 10000

tc qdisc del dev $LOCAL_ETH root
tc qdisc add dev $LOCAL_ETH root netem delay $DELAY $DELAYVAR

iptables -t nat -F
iptables -t nat -A POSTROUTING -o $LOCAL_ETH -j MASQUERADE 
echo 1 > /proc/sys/net/ipv4/ip_forward





