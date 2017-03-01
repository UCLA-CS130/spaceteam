#define TEST_NOT_FOUND_HANDLER

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "not_found_handler.h"

class NotFoundHandlerTest : public ::testing::Test {
 protected:
  NotFoundHandlerTest() {
    status = not_found_handler.Init(uri_prefix, config);
  }

  NotFoundHandler not_found_handler;
  const std::string uri_prefix;
  const NginxConfig config;
  const std::string EXPECTED_RESPONSE = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body><h1>404 Not Found</h1></body></html>";
  RequestHandler::Status status;
};

TEST_F(NotFoundHandlerTest, InitTest) {
  EXPECT_EQ(status, RequestHandler::Status::OK);
}

TEST_F(NotFoundHandlerTest, HandleProperRequestTest) {
  Request request;
  Response response;
  EXPECT_EQ(not_found_handler.HandleRequest(request, &response), RequestHandler::Status::OK);
  EXPECT_EQ(response.GetResponseCode(), Response::NOT_FOUND);
  EXPECT_EQ(response.ToString(), EXPECTED_RESPONSE);
}