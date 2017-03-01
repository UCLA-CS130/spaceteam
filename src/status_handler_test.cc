#define TEST_STATUS_HANDLER

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "echo_handler.h"
#include "status_handler.h"

class StatusHandlerTest : public ::testing::Test {
 protected:
  StatusHandlerTest() {
    status = status_handler.Init(uri_prefix, config);
    server_status.UpdateStatus("/echo", Response::OK);
    status_request = Request::Parse(STATUS_BUFFER);
    status_request->setServerStatus(&server_status);
  }

  EchoHandler echo_handler;
  StatusHandler status_handler;
  const std::string uri_prefix;
  const NginxConfig config;
  const std::string STATUS_BUFFER = "GET /status HTTP/1.1\r\nAccept-Language: en-us\r\n\r\nname=1";
  const std::string EXPECTED_STATUS_RESPONSE = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body><h1>Server Status</h1><h3>Handler Info</h3></br><h2>Total Requests: 1</h2><h5>Number of requests for \"/echo\"</h5><ul style=\"list-style-type:none\"><li>200: 1</li></ul></body></html>";
  RequestHandler::Status status;
  std::unique_ptr<Request> echo_request;
  std::unique_ptr<Request> status_request;
  Response echo_response;
  Response status_response;
  ServerStatus server_status;
};

TEST_F(StatusHandlerTest, InitTest) {
  EXPECT_EQ(status, RequestHandler::Status::OK);
}

TEST_F(StatusHandlerTest, HandleProperRequestTest) {
  EXPECT_EQ(status_handler.HandleRequest(*status_request, &status_response), RequestHandler::Status::OK);
  EXPECT_EQ(status_response.GetResponseCode(), Response::OK);
  EXPECT_EQ(status_response.ToString(), EXPECTED_STATUS_RESPONSE);
}