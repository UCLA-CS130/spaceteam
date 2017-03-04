#!/bin/bash

echo "Starting server"
./webserver test_config_302_redirect & sleep 1


echo "Make a request directly to www.ucla.edu"
curl -D proxheaders.txt www.ucla.edu > proxresponse.txt

echo "Make a request with 302 redirect"
curl -D backheaders.txt http://localhost:2020/proxy > headresponse.txt

if cmp -s proxresponse.txt headresponse.txt ; then
	rm proxheaders.txt
	rm proxresponse.txt
	rm backheaders.txt
	rm headresponse.txt

	kill %1
	echo "INTEGRATION TEST PASSED"
	exit 0
else
	rm proxheaders.txt
	rm proxresponse.txt
	rm backheaders.txt
	rm headresponse.txt

	kill %1
	echo "INTEGRATION TEST FAILED"
	exit 1
fi
