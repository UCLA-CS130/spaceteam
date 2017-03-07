# spaceteam Makefile

$CXX = g++
CXXFLAGS = -std=c++11 -Wall -Werror -pthread -isystem include
BOOST_FLAGS = -static-libgcc -static-libstdc++ -Wl,-Bstatic -lboost_thread -lboost_system -lboost_filesystem 
GTEST_DIR = googletest/googletest
GMOCK_DIR = googletest/googlemock
TEST_FLAGS = -std=c++11 -pthread
GTEST_FLAGS = $(TEST_FLAGS) -isystem $(GTEST_DIR)/include -I$(GTEST_DIR)
GMOCK_FLAGS = $(GTEST_FLAGS) -isystem $(GMOCK_DIR)/include -I$(GMOCK_DIR)

CLASSES = config_parser/config_parser src/server src/connection src/request src/response src/server_status \
					src/request_handler src/echo_handler src/proxy_handler src/static_handler src/not_found_handler src/status_handler src/holding_handler

SOURCES = $(CLASSES:=.cc)
OBJECTS = $(CLASSES:=.o)
# TODO: make tests of the rest of the .cc files. For now, using ACTUAL_TESTS instead of TESTS
TESTS = $(CLASSES:=_test.cc)
ACTUAL_TESTS = config_parser/config_parser_test src/connection_test src/request_test src/server_test src/not_found_handler_test src/echo_handler_test src/static_handler_test src/status_handler_test src/proxy_handler_test
ACTUAL_TESTS_SOURCE = $(ACTUAL_TESTS:=.cc)
GCOV = config_parser/config_parser.cc src/connection.cc src/request.cc src/server.cc

all: webserver

webserver: $(OBJECTS) src/main.cc
	$(CXX) $(CXXFLAGS) $^ -o $@ $(BOOST_FLAGS)

check: webserver $(ACTUAL_TESTS)
	for test in $^ ; do ./$$test ; done
	./server_integration_test.sh
	python multithread_test.py

reverse_proxy_integration_test:
	./reverse_proxy_integration_test.sh

reverse_proxy_302_test:
	./reverse_proxy_302_test.sh

gcov: CXXFLAGS += -fprofile-arcs -ftest-coverage
gcov: TEST_FLAGS += -fprofile-arcs -ftest-coverage
gcov: clean check
	for test in $(GCOV) ; do gcov -r ./$$test ; done


%_test: %_test.cc libgtest.a libgmock.a $(OBJECTS)
	$(CXX) $(GMOCK_FLAGS) -isystem include $(OBJECTS) $< $(GMOCK_DIR)/src/gmock_main.cc libgmock.a $(BOOST_FLAGS) -o $@


libgtest.a:
	$(CXX) $(GTEST_FLAGS) -c $(GTEST_DIR)/src/gtest-all.cc
	ar -rv $@ gtest-all.o

libgmock.a:
	$(CXX) $(GMOCK_FLAGS) -c $(GTEST_DIR)/src/gtest-all.cc
	$(CXX) $(GMOCK_FLAGS) -c $(GMOCK_DIR)/src/gmock-all.cc
	ar -rv $@ gmock-all.o gtest-all.o

%.o: $.cc
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) *.o *~ *.a *.gcov *.gcda *.gcno webserver
	$(RM) src/*.o src/*~ src/*.gcda src/*.gcno src/server_test src/request_test src/connection_test src/not_found_handler_test src/echo_handler_test src/static_handler_test src/status_handler_test src/proxy_handler_test
	$(RM) config_parser/*.o config_parser/*~ config_parser/*.gcda config_parser/*.gcno config_parser/config_parser_test

docker_image:
	docker build -t webserver.build .
	docker run webserver.build > webserver_binary.tar

.PHONY: all gcov check clean
