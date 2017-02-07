#define TEST_REQUEST_PARSER

#include "gtest/gtest.h"
#include "request_parser.h"

class RequestParserTest : public ::testing::Test {
 protected:
  RequestParserTest() {
    parser_ = new RequestParser();
  }

};

TEST_F(RequestParserTest, correctRequest) {

}