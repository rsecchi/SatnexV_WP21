#!/bin/bash


IF=$1

RXprev=$(ifconfig $IF | grep "RX p" | awk '{print $5}')
TXprev=$(ifconfig $IF | grep "TX p" | awk '{print $5}')
while true;
do
    RXcur=$(ifconfig $IF | grep "RX p" | awk '{print $5}')
    TXcur=$(ifconfig $IF | grep "TX p" | awk '{print $5}')
    RXdelta=$(expr 8 \* \( $RXcur - $RXprev \) / 1000)
    TXdelta=$(expr 8 \* \( $TXcur - $TXprev \) / 1000)
    RXprev=$RXcur
    TXprev=$TXcur
    echo $RXdelta  $TXdelta
    sleep 1 
done | feedgnuplot --stream --xlen 60 --terminal 'dumb 80,30' --lines


