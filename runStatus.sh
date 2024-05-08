#!/bin/bash

cd /home/nvidia/System_Monitoring/UART
sudo jetson_clocks
sudo taskset -c 0 ./UARTSend &
sudo renice -n -20 -p $(pgrep -f "UARTSend" | tail -n 1 | awk '{print $1}')
