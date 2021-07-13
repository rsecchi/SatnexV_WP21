#!/bin/bash

# Name on the local interface on public network
LOCAL_ETH=wlp2s0

VPN_TAP_NAME=tap0

# Creates permanent TAP for ns-3
tunctl -t tap-left
tunctl -t tap-right

brctl addbr br1
brctl addif br1 $VPN_TAP_NAME 
brctl addif br1 tap-left

ifconfig tap-left up
ifconfig tap-right 10.1.2.1/24 up
ifconfig br1 up
ip route add 10.1.1.0/24 via 10.1.2.2
ifconfig $VPN_TAP_NAME  up

iptables -t nat -A POSTROUTING -o $LOCAL_ETH -j MASQUERADE 
echo 1 > /proc/sys/net/ipv4/ip_forward



