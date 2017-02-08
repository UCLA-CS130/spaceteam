# spaceteam Makefile

CC = g++
CFLAGS = -std=c++11 -Wall -Werror
GTEST_DIR = googletest/googletest
TEST_CFLAGS = -std=c++0x -isystem $(GTEST_DIR)/include -pthread $(GCOV_FLAGS)
GCOV_FLAGS = -fprofile-arcs -ftest-coverage

ALL_BIN = $(CONFIG) $(CONFIG_TEST) $(SERVER) $(SERVER_TEST) $(CONNECTION) $(CONNECTION_TEST) webserver
CONFIG = config_parser
CONFIG_TEST = config_parser_test
SERVER = server
SERVER_TEST = server_test
CONNECTION = connection
CONNECTION_TEST = connection_test
REQUEST_PARSER = request_parser
REQUEST_PARSER_TEST = request_parser_test
INTEGRATION_TEST = server_integration_test.sh

PARSER_SOURCE = config_parser.cc config_parser_main.cc
SERVER_SOURCE = server.cc main.cc config_parser.cc
CONNECTION_SOURCE = connection.cc request_parser.cc request_handler.cc response.cc header.cc
REQUEST_PARSER_TEST_SOURCE = request_parser_test.cc request_parser.cc $(GTEST_DIR)/src/gtest_main.cc  
CONFIG_TEST_SOURCE = config_parser_test.cc config_parser.cc $(GTEST_DIR)/src/gtest_main.cc
SERVER_TEST_SOURCE = server_test.cc server.cc $(GTEST_DIR)/src/gtest_main.cc config_parser.cc
CONNECTION_TEST_SOURCE = connection_test.cc connection.cc $(GTEST_DIR)/src/gtest_main.cc request_parser.cc request_handler.cc response.cc header.cc

all: parser webserver

parser:	$(PARSER_SOURCE)
	$(CC) $(CFLAGS) -g -o $(CONFIG) $(PARSER_SOURCE)

webserver: $(SERVER_SOURCE) $(CONNECTION_SOURCE)
	$(CC) $(CFLAGS) $^ -o $@ -lboost_system -lboost_filesystem

check: webserver check_config check_server check_connection integ_test check_request_parser
	gcov -r connection.cc
	gcov -r server.cc
	gcov -r config_parser.cc
	gcov -r request_parser.cc

check_config: config_test
	./$(CONFIG_TEST)

check_server: server_test
	./$(SERVER_TEST)

check_request_parser: request_parser_test
	./$(REQUEST_PARSER_TEST)

integ_test:
	./$(INTEGRATION_TEST)

check_connection: connection_test
	./$(CONNECTION_TEST)

config_test: $(CONFIG_TEST_SOURCE) libgtest.a
	$(CC) $(TEST_CFLAGS) $^ libgtest.a -o $(CONFIG_TEST)

server_test: $(SERVER_TEST_SOURCE) $(CONNECTION_TEST_SOURCE) libgtest.a
	$(CC) $(TEST_CFLAGS) $^ -o $(SERVER_TEST) -lboost_system -lboost_filesystem

connection_test: $(CONNECTION_TEST_SOURCE) libgtest.a
	$(CC) $(TEST_CFLAGS) $^ -o $(CONNECTION_TEST) -lboost_system -lboost_filesystem

request_parser_test: $(REQUEST_PARSER_TEST_SOURCE) libgtest.a
	$(CC) $(TEST_CFLAGS) $^ -o $(REQUEST_PARSER_TEST) -lboost_system

libgtest.a: gtest-all.o
	ar -rv $@ $^

gtest-all.o: $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/include/gtest/gtest.h
	$(CC) $(TEST_CFLAGS) -I$(GTEST_DIR) -c $(GTEST_DIR)/src/gtest-all.cc

clean:
	$(RM) *.o *~ *.a *.gcov *.gcda *.gcno $(ALL_BIN)

.PHONY: all parser check clean
