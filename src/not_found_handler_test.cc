#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "not_found_handler.h"

class NotFoundHandlerTest : public ::testing::Test {
 protected:
  NotFoundHandlerTest() {
    const std::string uri_prefix;
    const NginxConfig config;
    status_ = not_found_handler_.Init(uri_prefix, config);
  }

  NotFoundHandler not_found_handler_;
  const std::string EXPECTED_RESPONSE = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body><h1>404 Not Found</h1></body></html>";
  RequestHandler::Status status_;
};

TEST_F(NotFoundHandlerTest, InitTest) {
  EXPECT_EQ(status_, RequestHandler::Status::OK);
}

TEST_F(NotFoundHandlerTest, HandleProperRequestTest) {
  Request request;
  Response response;
  EXPECT_EQ(not_found_handler_.HandleRequest(request, &response), RequestHandler::Status::OK);
  EXPECT_EQ(response.GetResponseCode(), Response::NOT_FOUND);
  EXPECT_EQ(response.ToString(), EXPECTED_RESPONSE);
}
