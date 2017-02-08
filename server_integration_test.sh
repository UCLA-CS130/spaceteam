#!/bin/bash
# assistance from http://stackoverflow.com/questions/25320928/how-to-capture-the-output-of-curl-to-variable-in-bash
# in order to capture the curl command

local_url="http://localhost:1890"

# === Integration Test 1 ===
# Checking that the server starts and returns a response.

# Start the webserver
./webserver test_config &

# Tests for status code
output=$(curl -I $local_url | head -n 1| cut -d $' ' -f2)

# Checks the "OK" status code from HTTP
if [ $output -eq "200" ];
	then
		echo "SUCCESS: RECEIVED A RESPONSE"
	else
		echo "FAILED: TO RECEIVE A RESPONSE"
fi

# Stop the webserver
# kill %1

# === Integration Test 2 ===
# Checking that the server can return a static file
# Start the webserver
# ./webserver test_config & 

output2=$(curl http://localhost:1890/static/test.html | diff example_files/test.html - )

# Checks that the diff produces no output, since the files should be the same.
if [$output2 -eq ""];
	then
		echo "SUCCESS: SERVED TEST.HTML FILE"
	else
		echo "FAILED: TO SERVE TEST.HTML FILE"
fi

#Stop the webserver
# kill %1

# === Integration Test 3 ===
# Checking that the server can't return a static file, returning 404
# Start the webserver
# ./webserver test_config & 

output3=$(curl -I http://localhost:1890/static/notreal.html | head -n 1 | cut -d $' ' -f2)

# Checks that the diff produces no output, since the files should be the same.
if [ $output3 -eq "404" ];
	then
		echo "SUCCESS: RETURNED 404 NOT FOUND ERROR"
	else
		echo "FAILED: DID NOT RETURN 404 ERROR"
fi

#Stop the webserver
kill %1
