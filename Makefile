# spaceteam Makefile

CC = g++
CFLAGS = -std=c++11 -Wall -Werror
GTEST_DIR = googletest/googletest
TEST_CFLAGS = -std=c++0x -isystem $(GTEST_DIR)/include -pthread

CONFIG = config_parser
TEST_CONFIG = config_parser_test
SERVER = server
TEST_SERVER = server_test

PARSER_SOURCE = config_parser.cc config_parser_main.cc
SERVER_SOURCE = connection.cc server.cc main.cc config_parser.cc
TEST_CONFIG_SOURCE = config_parser_test.cc config_parser.cc $(GTEST_DIR)/src/gtest_main.cc
TEST_SERVER_SOURCE = server_test.cc server.cc $(GTEST_DIR)/src/gtest_main.cc config_parser.cc connection.cc

all: parser webserver

parser:	$(PARSER_SOURCE)
	$(CC) $(CFLAGS) -g -o $(CONFIG) $(PARSER_SOURCE)

webserver: $(SERVER_SOURCE)
	$(CC) $(CFLAGS) $^ -o $@ -lboost_system

check_config: tests_config
	./$(TEST_CONFIG)

check_server: test_server
	./$(TEST_SERVER)

test_config: $(TEST_CONFIG_SOURCE) libgtest.a
	$(CC) $(TEST_CFLAGS) $(TEST_CONFIG_SOURCE) libgtest.a -o $(TEST_CONFIG)

test_server: $(TEST_SERVER_SOURCE) libgtest.a
	$(CC) $(TEST_CFLAGS) $(TEST_SERVER_SOURCE) libgtest.a -o $(TEST_SERVER) -lboost_system


libgtest.a: gtest-all.o
	ar -rv $@ $^

gtest-all.o: $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/include/gtest/gtest.h
	$(CC) $(TEST_CFLAGS) -I$(GTEST_DIR) -c $(GTEST_DIR)/src/gtest-all.cc

clean:
	$(RM) *.o *~ *.a $(CONFIG) $(TEST_CONFIG) $(SERVER) $(TEST_SERVER) webserver

.PHONY: all parser check clean
