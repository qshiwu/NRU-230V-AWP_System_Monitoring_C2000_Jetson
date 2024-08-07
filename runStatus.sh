#!/bin/bash

function setGPO_JP6(){
# $1=gpo_id $2=gpo_value
    PIN=$1
    pkill -f -- "$(sudo gpiofind $PIN)"
    gpioset -m signal $(sudo gpiofind $PIN)=$2 &
}

#Reset C2000 UART
setGPO_JP6 PAA.07 1
sleep 3
setGPO_JP6 PAA.07 0


cd /home/nvidia/System_Monitoring/UART
sudo jetson_clocks
sudo ./UARTSend &
