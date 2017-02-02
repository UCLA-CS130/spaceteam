#!/bin/bash

expected="who knows"

# Start the webserver
./webserver --port 1880 &

# Run the test
if [ curl http://localhost:1880 | grep $expected x = 'x'];
	then
		echo "FAILED"
	else
		echo "SUCCESS"
fi

# Stop the webserver
kill %1