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

######## iptable config #################
# Don't route between "tun0" and "tap"
iptables -t nat -F
iptables -F
iptables -A FORWARD -i tun0 -j DROP
iptables -A FORWARD -o tun0 -j DROP
iptables -A FORWARD -i tap-left -j DROP
iptables -A FORWARD -o tap-left -j DROP
iptables -t nat -A POSTROUTING -o $LOCAL_ETH -j MASQUERADE 

######## iptable config #################
# Route external packet to tap-right
ip rule add iif $LOCAL_ETH table 10
ip route add default via 10.1.2.2 table 10
echo 1 > /proc/sys/net/ipv4/ip_forward


# Start bridge and ns-3
$TUN2TAP &

cd $NS3_DIR 
./waf --run satdelay


