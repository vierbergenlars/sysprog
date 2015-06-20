#!/bin/sh
set -e
./logger&
./server 1234&
SERVER_PID=$!
sleep 1
./sensor_node 1 5 127.0.0.1 1234&
./sensor_node 2 5 127.0.0.1 1234&
./sensor_node 5 10 127.0.0.1 1234&
echo "Press ^C to stop the server and sensor nodes"
tail -f gateway.log&
TAIL_PID=$!
trap "kill $TAIL_PID" EXIT INT
wait $SERVER_PID
