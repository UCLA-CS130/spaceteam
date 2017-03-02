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
    status_ = static_handler.Init(uri_prefix_, config_);
    request_ = Request::Parse(TEST_BUFFER);
  }

  bool ParseString(const std::string config_string) {
    std::stringstream config_stream(config_string);
    return parser_.Parse(&config_stream, &config_);
  }
  NginxConfigParser parser_;
  NginxConfig config_;

  StaticHandler static_handler_;
  const std::string uri_prefix_ = "/static";
  const std::string TEST_BUFFER = "GET /static/test.html HTTP/1.1\r\nUser-Agent: curl/7.35.0\r\nHost: localhost:2020\r\nAccept: */*\r\n\r\n";
  std::string EXPECTED_RESPONSE = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>Server Test</title>\r\n</head>\r\n<body>\r\n<h1>Testing testing 1 2 3...</h1>\r\n<p>Looks like the server is working!</p>\r\n</body>\r\n</html>";
  RequestHandler::Status status_;
  std::unique_ptr<Request> request_;
  Response response_;
};

TEST_F(StaticHandlerTest, InitTest) {
  EXPECT_EQ(status_, RequestHandler::Status::OK);
}

TEST_F(StaticHandlerTest, NullResponse) {
  EXPECT_EQ(static_handler_.HandleRequest(*request_, nullptr), RequestHandler::Status::ERROR);
}

TEST_F(StaticHandlerTest, HandleProperRequestTest) {
  EXPECT_EQ(static_handler_.HandleRequest(*request_, &response_), RequestHandler::Status::OK);
  EXPECT_EQ(response_.GetResponseCode(), Response::OK);
  EXPECT_EQ(response_.ToString(), EXPECTED_RESPONSE);
}
