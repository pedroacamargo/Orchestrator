#!/bin/bash

# Number of repetitions
repetitions=20

# Variable to store total response time
total_response_time=0

for ((rep=1; rep<=$repetitions; rep++))
do
    # Clear the completed.txt file
    > tmp/completed.txt

    if [ -p "fifo_server" ]; then
        echo "Fifo is opened, remaking everything from scratch..."
        make -s clean && make -s
    else
        echo "File does not exist, able to start orchestrator and clients..."
        make -s
    fi

    # Start a new tmux session named "my_session"
    tmux new-session -d -s my_session

    echo "Choose the number of parallel tasks: "
    read parallel_tasks
    echo "Choose the scheduling algorithm <FCFS || SJF>: "
    read scheduling_algorithm
    echo "Choose number of clients to start: "
    read number_of_clients

    # Split the window horizontally
    tmux split-window -h

    # Send a command to the second pane (pane index 1)
    tmux send-keys -t 1 "./orchestrator output $parallel_tasks $scheduling_algorithm" Enter

    sum=0
    for (( i=0; i < $number_of_clients; i++ ))
    do
        random_number=$((1 + $RANDOM % 10))
        tmux send-keys -t 2 "./client execute $random_number -u 'sleep $random_number'" Enter
        sum=$((sum + random_number))
    done

    # Calculate the average time to wait for all clients to finish
    delay=$((($sum / $parallel_tasks) + 10))
    tmux send-keys -t 2 "echo The response will be available in $delay seconds..." Enter

    (sleep $delay; tmux send-keys -t 1 C-z; tmux kill-session -t my_session; echo All processes were finished after "$(tail -n 1 tmp/completed.txt | awk "{print \$NF}")" milliseconds) &

    # Attach to the tmux session
    tmux attach-session -t my_session

    # Calculate the response time for this repetition
    response_time=$(tail -n 1 tmp/completed.txt | awk '{print $NF}')
    total_response_time=$((total_response_time + response_time))

    # Print the response time for this repetition
    echo "Response time for repetition $rep: $response_time milliseconds"
done

# Calculate the average response time
average_response_time=$((total_response_time / repetitions))

echo "Average response time across $repetitions repetitions: $average_response_time milliseconds"
