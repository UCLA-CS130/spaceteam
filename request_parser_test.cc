#define TEST_REQUEST_PARSER
  
#include "gtest/gtest.h"
#include "request_parser.h"
#include "request.h"

#define BUFFER_SIZE 1024 

class RequestParserTest : public ::testing::Test {
  protected:
  RequestParserTest() {
  }

  void runParse() {
    std::tie(result, std::ignore) = parser_.parse(
        request, test_buffer.begin(), test_buffer.end()); 
  }

  void setUpRequest(std::string buffer) {
    test_buffer = buffer;
    runParse();
  }

  void properRequest() {
    setUpRequest("GET /echo/hello/there.html HTTP/1.1\r\n\r\n");
  }

  std::string test_buffer;
  RequestParser parser_;
  Request request;
  RequestParser::result_type result;
};

TEST_F(RequestParserTest, EmptyRequest) { 
  std::tie(result, std::ignore) = parser_.parse(
      request, test_buffer.begin(), test_buffer.end());
  EXPECT_EQ(result, RequestParser::result_type::indeterminate);
}

TEST_F(RequestParserTest, CorrectRequest) { 
  properRequest();
  EXPECT_EQ(result, RequestParser::result_type::good);
}

// Correct Handler + File Paths
TEST_F(RequestParserTest, CorrectPathsHandlerRequest) {
  properRequest();
  EXPECT_EQ(request.handler_path, "/echo");
  EXPECT_EQ(request.file_path, "/hello/there.html");
  EXPECT_EQ(result, RequestParser::result_type::good);
}

TEST_F(RequestParserTest, IncorrectRequest) { 
  setUpRequest("asdfj<kl;");
  EXPECT_EQ(result, RequestParser::result_type::bad);
}
