#!/bin/bash

# Name on the local interface on public network
LOCAL_ETH=wlp2s0
LOCAL_TUN=tun0

CMD="./netem_config <delay_ms> <delay_var_ms> <bandwidth_bps> <buffer_size_bytes>"

# average propagation delay (the RTT is 2xDELAY)
# DELAY=1ms 
DELAY=${1?$CMD}

# uniform distribution
# DELAY_VAR=1ms
DELAY_VAR=${2?$CMD}

# downlink bandwidth
# BANDWIDTH=5Mbit     
BANDWIDTH=${3?$CMD}

# buffer size in packets
# QLEN=50
BUFFER_SIZE=${4?$CMD}


####################################################################

ifconfig $LOCAL_TUN txqueuelen 1000

tc qdisc del dev $LOCAL_TUN root
tc qdisc add dev $LOCAL_TUN root tbf rate $BANDWIDTH  buffer 1600 limit $BUFFER_SIZE

# This will cause a delay also to returning VPN packets
tc qdisc del dev $LOCAL_ETH root
tc qdisc add dev $LOCAL_ETH root netem delay $DELAY $DELAYVAR

iptables -t nat -F
# iptables -t nat -A POSTROUTING -o $LOCAL_ETH -j MASQUERADE 
echo 1 > /proc/sys/net/ipv4/ip_forward





