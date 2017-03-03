#!/bin/bash

echo "Starting server 1"
./webserver test_config_2 & sleep 1
echo "Starting server 2"
./webserver test_config_3 & sleep 2

echo "Make a request to proxy"
curl -D proxheaders.txt http://localhost:2040/ > proxresponse.txt

echo "Make a request directly to backend"
curl -D backheaders.txt http://localhost:2030/ > headresponse.txt

if cmp -s proxresponse.txt headresponse.txt ; then
	rm proxheaders.txt
	rm proxresponse.txt
	rm backheaders.txt
	rm headresponse.txt

	kill %1
	kill %2
	echo "INTEGRATION TEST PASSED"
	exit 0
else
	rm proxheaders.txt
	rm proxresponse.txt
	rm backheaders.txt
	rm headresponse.txt

	kill %1
	kill %2
	echo "INTEGRATION TEST FAILED"
	exit 1
fi
