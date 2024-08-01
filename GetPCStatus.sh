#!/bin/bash
#03-08-2024 07:49:04 RAM 1438/30622MB (lfb 6704x4MB) SWAP 0/15311MB (cached 0MB) CPU [2%@729,0%@729,4%@729,0%@729,2%@729,0%@729,0%@729,2%@729] EMC_FREQ 0% GR3D_FREQ 0% CV0@-256C CPU@44.437C Tboard@36C SOC2@42.562C Tdiode@35.75C SOC0@42.062C CV1@-256C GPU@-256C tj@44.437C SOC1@43.031C CV2@-256C

TXTPath="$(pwd)/tegrastats.txt"

rm $TXTPath
tegrastats --interval 400 --logfile $TXTPath &
sleep 0.5
kill $(pgrep -f tegrastats)
PrintRAM=$(cat $TXTPath | grep -o "RAM [0-9/MB]*")

PrintCPU=$(cat $TXTPath | grep -o 'CPU \[[^]]*\]')
PrintCPUTemp=$(cat $TXTPath | grep -o 'cpu@-\?[0-9]\+\.\?[0-9]*C')
PrintGPUTemp=$(cat $TXTPath | grep -o 'gpu@-\?[0-9]\+\.\?[0-9]*C')
if [ -z "$PrintGPUTemp" ]; then
    PrintGPUTemp="gpu@0C"
fi


RAM=$(echo $PrintRAM | awk -F' ' '{print $2}' | awk -F'[\/]' '{print $1}')

#PrintCPU="CPU [1%@729,2%@729,3%@729,4%@729,5%@729,6%@729,7%@729,8%@729]" 
CPU0=$(echo "$PrintCPU" | awk -F'[,]' '{print $1}' | awk -F'[@]' '{print $1}' | awk -F'[\[]' '{print $2}')
CPU1=$(echo "$PrintCPU" | awk -F'[,]' '{print $2}' | awk -F'[@]' '{print $1}')
CPU2=$(echo "$PrintCPU" | awk -F'[,]' '{print $3}' | awk -F'[@]' '{print $1}')
CPU3=$(echo "$PrintCPU" | awk -F'[,]' '{print $4}' | awk -F'[@]' '{print $1}')
CPU4=$(echo "$PrintCPU" | awk -F'[,]' '{print $5}' | awk -F'[@]' '{print $1}')
CPU5=$(echo "$PrintCPU" | awk -F'[,]' '{print $6}' | awk -F'[@]' '{print $1}')
CPU6=$(echo "$PrintCPU" | awk -F'[,]' '{print $7}' | awk -F'[@]' '{print $1}')
CPU7=$(echo "$PrintCPU" | awk -F'[,]' '{print $8}' | awk -F'[@]' '{print $1}')

CPUTemp=$(echo "$PrintCPUTemp" | awk -F'[@]' '{print $2}')
GPUTemp=$(echo "$PrintGPUTemp" | awk -F'[@]' '{print $2}')


SendUART1="PC_STATUS1,$RAM,$CPU0,$CPU1,$CPU2,$CPU3,$CPU4,$CPU5,$CPU6,$CPU7"
SendUART2="PC_STATUS2,$CPUTemp,$GPUTemp"


echo "$SendUART1"
echo "$SendUART2"


