#!/bin/bash
#03-08-2024 07:49:04 RAM 1438/30622MB (lfb 6704x4MB) SWAP 0/15311MB (cached 0MB) CPU [2%@729,0%@729,4%@729,0%@729,2%@729,0%@729,0%@729,2%@729] EMC_FREQ 0% GR3D_FREQ 0% CV0@-256C CPU@44.437C Tboard@36C SOC2@42.562C Tdiode@35.75C SOC0@42.062C CV1@-256C GPU@-256C tj@44.437C SOC1@43.031C CV2@-256C

TXTPath="$(pwd)/tegrastats.txt"

rm $TXTPath
tegrastats --interval 400 --logfile $TXTPath &
sleep 0.5
kill $(pgrep -f tegrastats)
PrintRAM=$(cat $TXTPath | grep -o "RAM [0-9/MB]*")

PrintCPU=$(cat $TXTPath | grep -o 'CPU \[[^]]*\]')
PrintCV0Temp=$(cat $TXTPath | grep -o 'CV0@-\?[0-9]\+\.\?[0-9]*C')
PrintCPUTemp=$(cat $TXTPath | grep -o 'CPU@-\?[0-9]\+\.\?[0-9]*C')
PrintTboardTemp=$(cat $TXTPath | grep -o 'Tboard@-\?[0-9]\+\.\?[0-9]*C')
PrintSOC2Temp=$(cat $TXTPath | grep -o 'SOC2@-\?[0-9]\+\.\?[0-9]*C')
PrintTdiodeTemp=$(cat $TXTPath | grep -o 'Tdiode@-\?[0-9]\+\.\?[0-9]*C')
PrintSOC0Temp=$(cat $TXTPath | grep -o 'SOC0@-\?[0-9]\+\.\?[0-9]*C')
PrintCV1Temp=$(cat $TXTPath | grep -o 'CV1@-\?[0-9]\+\.\?[0-9]*C')
PrintGPUTemp=$(cat $TXTPath | grep -o 'GPU@-\?[0-9]\+\.\?[0-9]*C')
PrinttjTemp=$(cat $TXTPath | grep -o 'tj@-\?[0-9]\+\.\?[0-9]*C')
PrintSOC1Temp=$(cat $TXTPath | grep -o 'SOC1@-\?[0-9]\+\.\?[0-9]*C')
PrintCV2Temp=$(cat $TXTPath | grep -o 'CV2@-\?[0-9]\+\.\?[0-9]*C')

#echo "$PrintRAM"
#echo "$PrintCPU"
#echo "$PrintCV0Temp"
#echo "$PrintCPUTemp"
#echo "$PrintTboardTemp"
#echo "$PrintSOC2Temp"
#echo "$PrintTdiodeTemp"
#echo "$PrintSOC0Temp"
#echo "$PrintCV1Temp"
#echo "$PrintGPUTemp"
#echo "$PrinttjTemp"
#echo "$PrintSOC1Temp"
#echo "$PrintCV2Temp"
#
#echo "============================================"

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

CV0Temp=$(echo "$PrintCV0Temp" | awk -F'[@]' '{print $2}')
CPUTemp=$(echo "$PrintCPUTemp" | awk -F'[@]' '{print $2}')
TboardTemp=$(echo "$PrintTboardTemp" | awk -F'[@]' '{print $2}')
SOC2Temp=$(echo "$PrintSOC2Temp" | awk -F'[@]' '{print $2}')
TdiodeTemp=$(echo "$PrintTdiodeTemp" | awk -F'[@]' '{print $2}')
SOC0Temp=$(echo "$PrintSOC0Temp" | awk -F'[@]' '{print $2}')
CV1Temp=$(echo "$PrintCV1Temp" | awk -F'[@]' '{print $2}')
GPUTemp=$(echo "$PrintGPUTemp" | awk -F'[@]' '{print $2}')
tjTemp=$(echo "$PrinttjTemp" | awk -F'[@]' '{print $2}')
SOC1Temp=$(echo "$PrintSOC1Temp" | awk -F'[@]' '{print $2}')
CV2Temp=$(echo "$PrintCV2Temp" | awk -F'[@]' '{print $2}')

#echo "$RAM"
#
#echo "$CPU0"
#echo "$CPU1"
#echo "$CPU2"
#echo "$CPU3"
#echo "$CPU4"
#echo "$CPU5"
#echo "$CPU6"
#echo "$CPU7"
#
#echo "$CV0Temp" 
#echo "$CPUTemp"
#echo "$TboardTemp"
#echo "$SOC2Temp"
#echo "$TdiodeTemp"
#echo "$SOC0Temp"
#echo "$CV1Temp"
#echo "$GPUTemp"
#echo "$tjTemp"
#echo "$SOC1Temp"
#echo "$CV2Temp"

SendUART1="PC_STATUS1,$RAM,$CPU0,$CPU1,$CPU2,$CPU3,$CPU4,$CPU5,$CPU6,$CPU7\r\n"
SendUART2="PC_STATUS2,$CV0Temp,$CV1Temp,$CV2Temp,$CPUTemp,$GPUTemp,$TboardTemp,$SOC0Temp,$SOC1Temp,$SOC2Temp,$TdiodeTemp,$tjTemp\r\n"

echo -ne "SendUART1: $SendUART1"
echo -ne "SendUART2: $SendUART2"
sudo ./UARTSend "$SendUART1"
#sleep 0.01
sudo ./UARTSend "$SendUART2"


