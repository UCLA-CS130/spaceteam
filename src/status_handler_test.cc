#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "status_handler.h"

class MockResponse : public Response {};
class MockServerStatus : public ServerStatus {};

class StatusHandlerTest : public ::testing::Test {
 protected:
  StatusHandlerTest() {
    const std::string uri_prefix;
    const NginxConfig config;
    status_ = status_handler_.Init(uri_prefix, config);
    server_status_.UpdateStatus("/echo", MockResponse::OK);
    status_request_ = Request::Parse(STATUS_BUFFER);
    status_request_->setServerStatus(&server_status_);
  }

  StatusHandler status_handler_;
  const std::string STATUS_BUFFER = "GET /status HTTP/1.1\r\nAccept-Language: en-us\r\n\r\nname=1";
  const std::string EXPECTED_STATUS_RESPONSE = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body><h1>Server Status</h1><h3>Handler Info</h3></br><h2>Total Requests: 1</h2><h5>Number of requests for \"/echo\"</h5><ul style=\"list-style-type:none\"><li>200: 1</li></ul></body></html>";
  RequestHandler::Status status_;
  std::unique_ptr<Request> status_request_;
  MockResponse status_response_;
  MockServerStatus server_status_;
};

TEST_F(StatusHandlerTest, InitTest) {
  EXPECT_EQ(status_, RequestHandler::Status::OK);
}

TEST_F(StatusHandlerTest, HandleProperRequestTest) {
  EXPECT_EQ(status_handler_.HandleRequest(*status_request_, &status_response_), RequestHandler::Status::OK);
  EXPECT_EQ(status_response_.GetResponseCode(), MockResponse::OK);
  EXPECT_EQ(status_response_.ToString(), EXPECTED_STATUS_RESPONSE);
}
