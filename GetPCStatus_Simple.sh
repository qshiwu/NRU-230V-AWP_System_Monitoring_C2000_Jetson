#!/bin/bash
#03-08-2024 07:49:04 RAM 1438/30622MB (lfb 6704x4MB) SWAP 0/15311MB (cached 0MB) CPU [2%@729,0%@729,4%@729,0%@729,2%@729,0%@729,0%@729,2%@729] EMC_FREQ 0% GR3D_FREQ 0% CV0@-256C CPU@44.437C Tboard@36C SOC2@42.562C Tdiode@35.75C SOC0@42.062C CV1@-256C GPU@-256C tj@44.437C SOC1@43.031C CV2@-256C

TXTPath="$(pwd)/tegrastats.txt"

rm $TXTPath
tegrastats --interval 400 --logfile $TXTPath &
sleep 0.5
kill $(pgrep -f tegrastats)

PrintCPUTemp=$(cat $TXTPath | grep -o 'cpu@-\?[0-9]\+\.\?[0-9]*C')
PrintGPUTemp=$(cat $TXTPath | grep -o 'gpu@-\?[0-9]\+\.\?[0-9]*C')
if [ -z "$PrintGPUTemp" ]; then
    PrintGPUTemp="gpu@0C"
fi


CPUTemp=$(echo "$PrintCPUTemp" | awk -F'[@]' '{print $2}')
GPUTemp=$(echo "$PrintGPUTemp" | awk -F'[@]' '{print $2}')


SendUART3="PC_STATUS2,$CPUTemp,$GPUTemp"
echo "$SendUART3"

