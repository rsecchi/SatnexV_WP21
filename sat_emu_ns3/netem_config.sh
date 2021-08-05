#!/bin/bash

# Name on the local interface on public network
LOCAL_ETH=wlp2s0
LOCAL_TUN=tun0


DELAY=200ms         # average delay
DELAY_VAR=10ms      # uniform distribution
BANDWIDTH=2Mbit

tc qdisc del dev $LOCAL_TUN root
tc qdisc add dev $LOCAL_TUN root handle 1: tbf rate $BANDWIDTH  buffer 1600 limit 3000
tc qdisc add dev $LOCAL_TUN parent 1:1 handle 10: netem delay $DELAY $DELAY_VAR

iptables -t nat -F
iptables -t nat -A POSTROUTING -o $LOCAL_ETH -j MASQUERADE 
echo 1 > /proc/sys/net/ipv4/ip_forward





