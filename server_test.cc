#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "config_parser.h"
#include <vector>
#include <iostream>
#include <string>
#include "gtest/gtest.h"
#include "server.h"

// Server initialization test
TEST(ServerTest, SimpleServerTest) {
	boost::asio::io_service io_service;
	// "config" should eventually be moved to a global constant
	Server server(io_service, "test_config");
}