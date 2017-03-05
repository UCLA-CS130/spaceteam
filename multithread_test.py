# Help from : https://pymotw.com/2/threading/, https://www.tutorialspoint.com/python/python_networking.htm, https://github.com/UCLA-CS130/AAAAA/blob/master/test/integration_multithread_test.py

from os import system
from threading import Thread
import socket
import sys
import subprocess
import time

PORT = 2020
BUFFER_LEN = 1024
HOLD_REQUEST = "GET /hold HTTP/1.1"
STATIC_REQUEST = "GET /static/test.html HTTP/1.1\r\n\r\n"
EXPECTED_STATIC_RESPONSE = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Server Test</title>\r\n</head>\r\n<body>\r\n<h1>Testing testing 1 2 3...</h1>\r\n<p>Looks like the server is working!</p>\r\n</body>\r\n</html>"

# Sets up socket for connection
def setup_socket():
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	sock.connect(('localhost', PORT))
	return sock	

def receive(sock):
	data = sock.recv(BUFFER_LEN)
	sock.close()
	return data

def hold_connect():
	sock = setup_socket()
	sock.send(HOLD_REQUEST)
	sock.send("\r\n\r\n")
	receive(sock)
	return

def connect(request, expected_response, error_code):
	sock = setup_socket()
	sock.send(request)
	response = receive(sock)

	if (response != expected_response):
		print "Error: ", repr(response)
		error_code = 1
		return
	print "Connection Successful!"
	error_code = 0

def two_connections_test():
	print "Starting hold request thread"
	hold = Thread(target = hold_connect, args=())
	hold.start()

	time.sleep(3)

	print "Starting static request thread"
	static_error = 0
	static = Thread(target = connect, args=(STATIC_REQUEST, EXPECTED_STATIC_RESPONSE, static_error))
	static.start()
	static.join()

	# Kill the webserver process since the /hold connection is an infinite loop
	print "Closing webserver connection."
	system("pkill webserver")
	return static_error

def multiple_connections_test(numOfThreads):

	print "Starting static request thread"
	
	while(numOfThreads > 1):
		hold = Thread(target = hold_connect, args=())
		hold.start()
		time.sleep(3)	
		numOfThreads = numOfThreads - 1

	static_error = 0
	static = Thread(target = connect, args=(STATIC_REQUEST, EXPECTED_STATIC_RESPONSE, static_error))
	static.start()
	static.join()

	# Kill the webserver process since the /hold connection is an infinite loop
	print "Closing webserver connection."
	system("pkill webserver")
	return static_error

def start_server():
	print "Starting server..."
	p = subprocess.Popen("./webserver test_config", shell=True)

if __name__ == "__main__":
	# Give time for the server to start
	start_server()
	time.sleep(2)

	if (len(sys.argv) == 1):
		results = two_connections_test()	
	else:
		if (len(sys.argv) == 2):
			wow = int(sys.argv[1])
			results = multiple_connections_test(wow)

	time.sleep(2)

	if (results == 0):
		print "THREAD SUCCESS"
		exit(0)
	else:
		print "THREAD FAILURE"
		exit(1)
