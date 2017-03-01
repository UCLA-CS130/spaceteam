#define TEST_ECHO_HANDLER

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "echo_handler.h"

class EchoHandlerTest : public ::testing::Test {
 protected:
  EchoHandlerTest() {
    status = echo_handler.Init(uri_prefix, config);
    request = Request::Parse(TEST_BUFFER);
  }

  EchoHandler echo_handler;
  const std::string uri_prefix;
  const NginxConfig config;
  const std::string TEST_BUFFER = "GET /echo HTTP/1.1\r\nAccept-Language: en-us\r\n\r\nname=1";
  const std::string EXPECTED_RESPONSE = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET /echo HTTP/1.1\r\nAccept-Language: en-us\r\n\r\nname=1";
  RequestHandler::Status status;
  std::unique_ptr<Request> request;
};

TEST_F(EchoHandlerTest, InitTest) {
  EXPECT_EQ(status, RequestHandler::Status::OK);
}

TEST_F(EchoHandlerTest, HandleProperRequestTest) {
  Response response;
  EXPECT_EQ(echo_handler.HandleRequest(*request, &response), RequestHandler::Status::OK);
  EXPECT_EQ(response.GetResponseCode(), Response::OK);
  EXPECT_EQ(response.ToString(), EXPECTED_RESPONSE);
}