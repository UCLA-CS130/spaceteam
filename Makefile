# spaceteam Makefile

$CXX = g++
CXXFLAGS = -std=c++11 -Wall -Werror -isystem include
BOOST_FLAGS = -lboost_system -lboost_filesystem
GTEST_DIR = googletest/googletest
GMOCK_DIR = googletest/googlemock
TEST_FLAGS = -std=c++11 -pthread
GTEST_FLAGS = $(TEST_FLAGS) -isystem $(GTEST_DIR)/include -I$(GTEST_DIR)
GMOCK_FLAGS = $(GTEST_FLAGS) -isystem $(GMOCK_DIR)/include -I$(GMOCK_DIR)

CLASSES = config_parser/config_parser src/server src/connection src/request_parser src/request_handler src/response src/header
SOURCES = $(CLASSES:=.cc)
OBJECTS = $(CLASSES:=.o)
# TODO: make tests of the rest of the .cc files. For now, using ACTUAL_TESTS instead of TESTS
TESTS = $(CLASSES:=_test.cc)
ACTUAL_TESTS = config_parser/config_parser_test src/connection_test src/request_parser_test src/server_test
ACTUAL_TESTS_SOURCE = $(ACTUAL_TESTS:=.cc)
GCOV = config_parser/config_parser.cc src/connection.cc src/request_parser.cc src/server.cc

all: webserver

webserver: $(OBJECTS) src/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $@ $(BOOST_FLAGS)

check: webserver $(ACTUAL_TESTS)
	for test in $^ ; do ./$$test ; done
	./server_integration_test.sh

gcov: CXXFLAGS += -fprofile-arcs -ftest-coverage
gcov: TEST_FLAGS += -fprofile-arcs -ftest-coverage
gcov: clean check
	for test in $(GCOV) ; do gcov -r ./$$test ; done


%_test: %_test.cc libgtest.a libgmock.a $(OBJECTS)
	$(CXX) $(GTEST_FLAGS) -isystem include $(OBJECTS) $< $(GTEST_DIR)/src/gtest_main.cc libgtest.a $(BOOST_FLAGS) -o $@
	

libgtest.a:
	$(CXX) $(GTEST_FLAGS) -c $(GTEST_DIR)/src/gtest-all.cc
	ar -rv $@ gtest-all.o

libgmock.a:
	$(CXX) $(GTEST_FLAGS) -c $(GTEST_DIR)/src/gtest-all.cc
	$(CXX) $(GMOCK_FLAGS) -c $(GMOCK_DIR)/src/gmock-all.cc
	ar -rv $@ gmock-all.o

%.o: $.cc
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) *.o *~ *.a *.gcov *.gcda *.gcno webserver
	$(RM) src/*.o src/*~ src/*.gcda src/*.gcno src/server_test src/request_parser_test src/connection_test
	$(RM) config_parser/*.o config_parser/*~ config_parser/*.gcda config_parser/*.gcno config_parser/config_parser_test

.PHONY: all gcov check clean
