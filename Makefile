# spaceteam Makefile

CC = g++
CFLAGS = -std=c++11 -Wall -Werror
GTEST_DIR = googletest/googletest
TEST_CFLAGS = -std=c++0x -isystem $(GTEST_DIR)/include -pthread

TARGET = config_parser
TEST_TARGET = config_parser_test

PARSER_SOURCE = config_parser.cc config_parser_main.cc
SERVER_SOURCE = connection.cc server.cc main.cc config_parser.cc
TEST_SOURCE = config_parser_test.cc config_parser.cc $(GTEST_DIR)/src/gtest_main.cc

all: parser server

parser:	$(PARSER_SOURCE)
	$(CC) $(CFLAGS) -g -o $(TARGET) $(PARSER_SOURCE)

server: $(SERVER_SOURCE)
	$(CC) $(CFLAGS) $^ -o $@ -lboost_system

check: tests
	./$(TEST_TARGET)

tests: $(TEST_OBJECTS) libgtest.a
	$(CC) $(TEST_CFLAGS) $(TEST_SOURCE) libgtest.a -o $(TEST_TARGET)

libgtest.a: gtest-all.o
	ar -rv $@ $^

gtest-all.o: $(GTEST_DIR)/src/gtest-all.cc $(GTEST_DIR)/include/gtest/gtest.h
	$(CC) $(TEST_CFLAGS) -I$(GTEST_DIR) -c $(GTEST_DIR)/src/gtest-all.cc

clean:
	$(RM) *.o *~ *.a $(TARGET) $(TEST_TARGET) server

.PHONY: all parser check clean
