#!/bin/bash
# assistance from http://stackoverflow.com/questions/25320928/how-to-capture-the-output-of-curl-to-variable-in-bash
# in order to capture the curl command

url="http://localhost:1890"

# Start the webserver
./webserver test_config &

output=$(curl -I $url | head -n 1| cut -d $' ' -f2)

# Checks the "OK" status code from HTTP
if [ $output -eq "200" ];
	then
		echo "SUCCESS"
	else
		echo "FAILED"
fi

# Stop the webserver
kill %1
