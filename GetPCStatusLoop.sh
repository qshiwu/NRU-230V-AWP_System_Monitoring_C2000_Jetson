#!/bin/bash
condition=true
while [ $condition == true ]; do
    ./GetPCStatus.sh
    sleep 1
done
