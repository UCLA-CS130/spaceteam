# spaceteam Makefile

CC = g++
CFLAGS = -std=c++11 -Wall -Werror
GTEST_DIR = googletest/googletest
TEST_CFLAGS = -std=c++0x -isystem $(GTEST_DIR)/include -pthread

CONFIG = config_parser
CONFIG_TEST = config_parser_test
SERVER = server
SERVER_TEST = server_test
CONNECTION = connection
CONNECTION_TEST = connection_test

PARSER_SOURCE = config_parser.cc config_parser_main.cc
SERVER_SOURCE = connection.cc server.cc main.cc config_parser.cc
CONNECTION_SOURCE = connection.cc
CONFIG_TEST_SOURCE = config_parser_test.cc config_parser.cc $(GTEST_DIR)/src/gtest_main.cc
SERVER_TEST_SOURCE = server_test.cc server.cc $(GTEST_DIR)/src/gtest_main.cc config_parser.cc connection.cc
CONNECTION_TEST_SOURCE = connection_test.cc connection.cc $(GTEST_DIR)/src/gtest_main.cc

all: parser webserver

parser:	$(PARSER_SOURCE)
	$(CC) $(CFLAGS) -g -o $(CONFIG) $(PARSER_SOURCE)

webserver: $(SERVER_SOURCE)
	$(CC) $(CFLAGS) $^ -o $@ -lboost_system

check: check_config check_server check_connection

check_config: config_test
	./$(CONFIG_TEST)

check_server: server_test
	./$(SERVER_TEST)

check_connection: connection_test
	./$(CONNECTION_TEST)

config_test: $(CONFIG_TEST_SOURCE) libgtest.a
	$(CC) $(TEST_CFLAGS) $(CONFIG_TEST_SOURCE) libgtest.a -o $(CONFIG_TEST)

server_test: $(SERVER_TEST_SOURCE) libgtest.a
	$(CC) $(TEST_CFLAGS) $(SERVER_TEST_SOURCE) libgtest.a -o $(SERVER_TEST) -lboost_system

connection_test: $(CONNECTION_TEST_SOURCE) libgtest.a
	$(CC) $(TEST_CFLAGS) $(CONNECTION_TEST_SOURCE) libgtest.a -o $(CONNECTION_TEST) -lboost_system

libgtest.a: gtest-all.o
	ar -rv $@ $^

gtest-all.o: $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/include/gtest/gtest.h
	$(CC) $(TEST_CFLAGS) -I$(GTEST_DIR) -c $(GTEST_DIR)/src/gtest-all.cc

clean:
	$(RM) *.o *~ *.a $(CONFIG) $(CONFIG_TEST) $(SERVER) $(SERVER_TEST) $(CONNECTION) $(CONNECTION_TEST) webserver

.PHONY: all parser check clean
