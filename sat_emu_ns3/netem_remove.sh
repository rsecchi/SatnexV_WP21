#!/bin/bash

# Name on the local interface on public network
LOCAL_ETH=wlp2s0
LOCAL_TUN=tun0


#############################

ifconfig $LOCAL_TUN txqueuelen 100
tc qdisc del dev $LOCAL_TUN root
tc qdisc del dev $LOCAL_ETH root

iptables -t nat -F
echo 0 > /proc/sys/net/ipv4/ip_forward





