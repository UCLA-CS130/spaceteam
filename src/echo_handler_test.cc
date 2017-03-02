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
    std::string uri_prefix;  
    NginxConfig config; 
    status_ = echo_handler_.Init(uri_prefix, config);    
    request_ = Request::Parse(TEST_BUFFER);
  }

  EchoHandler echo_handler_;
  const std::string TEST_BUFFER = "GET /echo HTTP/1.1\r\nAccept-Language: en-us\r\n\r\nname=1";
  const std::string EXPECTED_RESPONSE = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nGET /echo HTTP/1.1\r\nAccept-Language: en-us\r\n\r\nname=1";
  RequestHandler::Status status_;
  std::unique_ptr<Request> request_;
};

TEST_F(EchoHandlerTest, InitTest) {
  EXPECT_EQ(status_, RequestHandler::Status::OK);
}

TEST_F(EchoHandlerTest, HandleProperRequestTest) {
  Response response;
  EXPECT_EQ(echo_handler_.HandleRequest(*request_, &response), RequestHandler::Status::OK);
  EXPECT_EQ(response.GetResponseCode(), Response::OK);
  EXPECT_EQ(response.ToString(), EXPECTED_RESPONSE);
}
