#!/bin/bash

# Name on the local interface on public network
LOCAL_ETH=enp0s3


# Creates permanent TAP for ns-3
tunctl -t tap-left
tunctl -t tap-right
openvpn --mktun --dev tap0

ifconfig tap-left up
ifconfig tap-right 10.1.2.1/24 up
ifconfig tap0 up

brctl addbr br1
brctl addif br1 tap0 
brctl addif br1 tap-left

ifconfig br1 up
ip route add 10.1.1.0/24 via 10.1.2.2

iptables -t nat -A POSTROUTING -o $LOCAL_ETH -j MASQUERADE 
echo 1 > /proc/sys/net/ipv4/ip_forward

cd /root/workspace/SatnexV_WP21/ns-allinone-3.33/ns-3.33/
./waf --run satdelay


