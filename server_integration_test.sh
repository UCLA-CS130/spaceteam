#!/bin/bash
# assistance from http://stackoverflow.com/questions/25320928/how-to-capture-the-output-of-curl-to-variable-in-bash
# in order to capture the curl command

# === Integration Test 1 ===
# Checking that the server starts and echos a response.

# Start the webserver
./webserver test_config &

# Tests for status code
output=$(curl -I http://localhost:2020/echo | head -n 1| cut -d $' ' -f2)

# Checks the "OK" status code from HTTP
if [ $output -eq "200" ];
	then
		echo "SUCCESS: RECEIVED A RESPONSE"
	else
		echo "FAILED: TO RECEIVE A RESPONSE"
fi

# # === Integration Test 2 ===
# # Checking that the server can return a static file
static_output=$(curl http://localhost:2020/static/test.html | diff example_files/test.html - )

# # Checks that the diff produces no output, since the files should be the same.
if [ -z $static_output ];
	then
		echo "SUCCESS: SERVED TEST.HTML FILE"
	else
		echo "FAILED: TO SERVE TEST.HTML FILE"
fi

# === Integration Test 3 ===
# Checking that the server can't return a static file, returning 404
not_found_output=$(curl -I http://localhost:2020/notreal.html | head -n 1 | cut -d $' ' -f2)

# Checks that the diff produces no output, since the files should be the same.
if [ $not_found_output -eq "404" ];
	then
		echo "SUCCESS: RETURNED 404 NOT FOUND ERROR"
	else
		echo "FAILED: DID NOT RETURN 404 ERROR"
fi

# === Integration Test 4 ===
# Checking that the server can handle multiple connections at once using multithreading

# thread_output=$(curl -I http://localhost:2020/hold | curl -I http://localhost:2020/hold | (sleep 20 && kill %1))

# # Checks that the diff produces no output, since the files should be the same.
# if [[ $thread_output == *"Waiting on HoldingHandler"* && $thread_output == *"200"*]];
# 	then
# 		echo "SUCCESS: HANDLED A SECOND CONNECTION SIMULTANEOUSLY"
# 	else
# 		echo "FAILED: DID NOT HANDLE A SECOND CONNECTION SIMULTANEOUSLY"
# fi

#Stop the webserver
kill %1
