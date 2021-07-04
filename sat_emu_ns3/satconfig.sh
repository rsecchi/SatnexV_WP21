#!/bin/bash


# start the VM first
# the VM IP address should be 10.1.1.1 and its gateway 10.1.1.2


VM_TAP_NAME=vmtap1

# Creates permanent TAP for ns-3
tunctl -t tap-left
tunctl -t tap-right

brctl addbr br1
brctl addif br1 $VM_TAP_NAME 
brctl addif br1 tap-left

ifconfig tap-left up
ifconfig tap-right 10.1.2.1/24 up
ifconfig br1 up
ip route add 10.1.1.0/24 via 10.1.2.2

echo 1 > /proc/sys/net/ipv4/ip_forward


