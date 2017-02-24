#define TEST_REQUEST
  
#include "gtest/gtest.h"
#include "request.h"

#define BUFFER_SIZE 1024 

class RequestTest : public ::testing::Test {
 protected:
  RequestTest() {
  }

  void runParse() {
    // TODO: Convert parser call from buffer beginning and end to a string... 
    request = Request::Parse(test_buffer); 
  }

  void setUpRequest(std::string buffer) {
    test_buffer = buffer;
    runParse();
  }

  void properRequest() {
    setUpRequest("GET /echo/hello/there.html HTTP/1.1\r\nAccept-Language: en-us\r\n\r\nname=1");
  }

  std::string test_buffer;
  std::unique_ptr<Request> request;
};

TEST_F(RequestTest, EmptyRequest) { 
  runParse();
  EXPECT_EQ(request->raw_request(), "");
  EXPECT_EQ(request->parsed_status(), Request::result_type::good);
}

TEST_F(RequestTest, CorrectRequest) { 
  properRequest();
  EXPECT_EQ(request->raw_request(), "GET /echo/hello/there.html HTTP/1.1\r\nAccept-Language: en-us\r\n\r\nname=1");
  EXPECT_EQ(request->method(), "GET");
  EXPECT_EQ(request->uri(), "/echo/hello/there.html");
  EXPECT_EQ(request->version(), "1.1");
  std::vector<std::pair<std::string, std::string>> req_headers = request->headers();
  std::pair<std::string, std::string> first_header_pair = req_headers[0];
  EXPECT_EQ(first_header_pair.first, "Accept-Language");
  EXPECT_EQ(first_header_pair.second, "en-us");
  EXPECT_EQ(request->body(), "name=1");
  EXPECT_EQ(request->parsed_status(), Request::result_type::good);
}

TEST_F(RequestTest, IncorrectRequest) { 
  setUpRequest("asdfj<kl;");
  // It should fail parsing as soon as it hits an invalid character.
  EXPECT_EQ(request->raw_request(), "asdfj<");
  EXPECT_EQ(request->parsed_status(), Request::result_type::bad);
}
