//Used help from neosbob/Razzle-Dazzle forked branch to create a fixture class: https://github.com/neosbob/Razzle-Dazzle/blob/proxy-handler/proxy_handler_test.cc

#include "gtest/gtest.h"
#include "proxy_handler.h"
#include "config_parser.h"
#include "response.h"
#include <string>
#include <limits.h>
#include <unistd.h>

class ProxyHandlerTest : public ::testing::Test {
protected:

	NginxConfigParser parser;
	NginxConfig conf;
	
	void parse(const std::string config_string) {
		std::stringstream configs(config_string);
		parser.Parse(&configs, &conf);
	}
	
};

TEST_F(ProxyHandlerTest, Init) {
	ProxyHandler proxy_handler;
	ASSERT_EQ(proxy_handler.Init("", conf), RequestHandler::Status::OK);
}

TEST_F(ProxyHandlerTest, HandleReq) {
	Request req;
	Response resp;
	ProxyHandler proxy_handler;
	
	std::string config_string = "host www.google.com;\r\n";
	config_string += "portno 80;\r\n";

	parse(config_string);

	ASSERT_EQ(proxy_handler.Init("", conf), RequestHandler::Status::OK);
	EXPECT_EQ(conf.statements_[0]->tokens_[0],"host");
	EXPECT_EQ(conf.statements_[0]->tokens_[1],"www.google.com");
	EXPECT_EQ(conf.statements_[1]->tokens_[0],"portno");
	EXPECT_EQ(conf.statements_[1]->tokens_[1],"80");

	ASSERT_EQ(proxy_handler.HandleRequest(req, &resp), RequestHandler::Status::OK);

}
