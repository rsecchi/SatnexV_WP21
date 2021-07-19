#!/bin/bash

# Name on the local interface on public network
LOCAL_ETH=enp0s3

# ns-3 directory 
NS3_DIR=/root/workspace/SatnexV_WP21/ns-allinone-3.33/ns-3.33/


TUN2TAP=/root/workspace/SatnexV_WP21/sat_emu_ns3/bridge

# Creates permanent TAP for ns-3
tunctl -t tap-left
tunctl -t tap-right

ifconfig tap-left  10.1.1.1/24 up
ifconfig tap-right 10.1.2.1/24 up

iptables -A FORWARD -i tun0 -j DROP
iptables -A FORWARD -o tun0 -j DROP
iptables -A FORWARD -i tap-left -j DROP
iptables -A FORWARD -o tap-left -j DROP

ip rule add iif $LOCAL_ETH table 10
ip route add default via 10.1.2.2 table 10

iptables -t nat -A POSTROUTING -o $LOCAL_ETH -j MASQUERADE 

echo 1 > /proc/sys/net/ipv4/ip_forward

$TUN2TAP &

cd $NS3_DIR 
./waf --run satdelay


