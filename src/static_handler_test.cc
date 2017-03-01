#define TEST_STATIC_HANDLER

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "static_handler.h"

class StaticHandlerTest : public ::testing::Test {
 protected:
  StaticHandlerTest() {
    ParseString("root ./example_files;");
    status = static_handler.Init(uri_prefix, config);
    request = Request::Parse(TEST_BUFFER);
  }

  bool ParseString(const std::string config_string) {
    std::stringstream config_stream(config_string);
    return parser.Parse(&config_stream, &config);
  }
  NginxConfigParser parser;
  NginxConfig config;

  StaticHandler static_handler;
  const std::string uri_prefix = "/static";
  const std::string TEST_BUFFER = "GET /static/test.html HTTP/1.1\r\nUser-Agent: curl/7.35.0\r\nHost: localhost:2020\r\nAccept: */*\r\n\r\n";
  std::string EXPECTED_RESPONSE = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Server Test</title>\r\n</head>\r\n<body>\r\n<h1>Testing testing 1 2 3...</h1>\r\n<p>Looks like the server is working!</p>\r\n</body>\r\n</html>";
  RequestHandler::Status status;
  std::unique_ptr<Request> request;
  Response response;
};

TEST_F(StaticHandlerTest, InitTest) {
  EXPECT_EQ(status, RequestHandler::Status::OK);
}

TEST_F(StaticHandlerTest, NullResponse) {
  EXPECT_EQ(static_handler.HandleRequest(*request, nullptr), RequestHandler::Status::ERROR);
}

TEST_F(StaticHandlerTest, HandleProperRequestTest) {
  EXPECT_EQ(static_handler.HandleRequest(*request, &response), RequestHandler::Status::OK);
  EXPECT_EQ(response.GetResponseCode(), Response::OK);
  EXPECT_EQ(response.ToString(), EXPECTED_RESPONSE);
}