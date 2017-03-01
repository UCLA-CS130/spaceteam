from os import system
from threading import Thread
import socket
import sys
import subprocess
import time

PORT = 2020
BUFFER_LEN = 1024
STATIC_REQUEST = "GET /static/test.html HTTP/1.1\r\n\r\n"
EXPECTED_STATIC_RESPONSE = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Server Test</title>\r\n</head>\r\n<body>\r\n<h1>Testing testing 1 2 3...</h1>\r\n<p>Looks like the server is working!</p>\r\n</body>\r\n</html>"

def set_socket():
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
	sock.connect(('localhost', PORT))
	return sock	

def hold_connect():
	sock = set_socket()
	sock.send("GET /hold HTTP/1.1")
	sock.send("\r\n\r\n")
	data = sock.recv(BUFFER_LEN)
	sock.close()
	return

def connect(REQ, EX_RESP, ec):
	sock = set_socket()
	sock.send(REQ)
	receive = sock.recv(BUFFER_LEN)
	sock.close()

	if (receive != EX_RESP):
		print "Error: ", repr(receive)
		ec = 1
		return
	print "Connection Successful!"
	ec = 0

def two_connections_test():
	print "Starting hold request thread"
	hold = Thread(target = hold_connect, args=())
	hold.start()

	print "Starting static request thread"
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

	two_connections_results = two_connections_test()
	time.sleep(2)

	if (two_connections_results == 0):
		print "THREAD SUCCESS"
		exit(0)
	else:
		print "THREAD FAILURE"
		exit(1)