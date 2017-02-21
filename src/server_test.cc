#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "config_parser.h"
#include <vector>
#include <iostream>
#include <string>
#include "gtest/gtest.h"
#include "server.h"
#include "server_info.h"

// Server initialization test
TEST(ServerTest, SimpleServerTest) {
	boost::asio::io_service io_service;
	// "config" should eventually be moved to a global constant
	Server *server = Server::makeServer(io_service, "test_config");
	EXPECT_TRUE(server != nullptr);
	delete server;
}

TEST(ServerInfoTest, GetServerInfo) {
	ServerInfo info;
	EXPECT_TRUE(Server::getServerInfo("test_config", &info));
	EXPECT_EQ(info.port, 1890);
	// TODO: REFACTORING should change these values.
	// EXPECT_EQ(info.echo_path_to_root_.size(), 1);
	// EXPECT_EQ(info.static_path_to_root_.size(), 1);
	// EXPECT_EQ(info.static_path_to_root_["/static"], "./example_files");
}

TEST(ServerInfoTest, UnexpectedConfig) {
	ServerInfo info;
	EXPECT_FALSE(Server::getServerInfo("test_config_bad", &info));
}

TEST(ServerInfoTest, UnexpectedHandlerConfig) {
	ServerInfo info;
	EXPECT_FALSE(Server::getServerInfo("test_config_bad2", &info));
}
