#!/bin/bash

if [ ! $# -ge 3 ]; then
    echo "Usage: $0 <paralel-tasks> <FCFS || SJF> <commands-number>"
    exit 1
fi

if [ -p "fifo_server" ]; then
    echo "Fifo is opened, remaking everything from scratch..."
    make clean && make
else
    echo "File does not exist, able to start orchestrator and clients..."
    make
fi

nohup konsole -e ./orchestrator output.txt $1 $2 &
echo "Orchestrator started with $1 parallel tasks and $2 scheduling algorithm"
sleep 2

echo "Starting clients..."
for (( i=0; i<$3; i++ ))
do
    random_number=$((1 + $RANDOM % 10))
    ./client execute $random_number -u "sleep $random_number"    
    sleep 0.2
done
