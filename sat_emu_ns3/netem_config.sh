#!/bin/bash

# Name on the local interface on public network
LOCAL_ETH=wlp2s0
LOCAL_TUN=tun0

# average propagation delay (the RTT is 2xDELAY)
# DELAY=1ms 
DELAY=${1?'./netem_config <delay> <delay_var> <bandwidth> <queue_len>'}

# uniform distribution
# DELAY_VAR=1ms
DELAY_VAR=${2?'./netem_config <delay> <delay_var> <bandwidth> <queue_len>'}

# downlink bandwidth
# BANDWIDTH=5Mbit     
BANDWIDTH=${3?'./netem_config <delay> <delay_var> <bandwidth> <queue_len>'}

# buffer size in packets
# QLEN=50
QLEN=${4?'./netem_config <delay> <delay_var> <bandwidth> <queue_len>'}


####################################################################

ifconfig $LOCAL_TUN txqueuelen $QLEN

tc qdisc del dev $LOCAL_TUN root
tc qdisc add dev $LOCAL_TUN root tbf rate $BANDWIDTH  buffer 1600 limit 10000

# This will cause a delay also to returning VPN packets
tc qdisc del dev $LOCAL_ETH root
tc qdisc add dev $LOCAL_ETH root netem delay $DELAY $DELAYVAR

iptables -t nat -F
iptables -t nat -A POSTROUTING -o $LOCAL_ETH -j MASQUERADE 
echo 1 > /proc/sys/net/ipv4/ip_forward





