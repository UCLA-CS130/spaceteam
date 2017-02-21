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
    setUpRequest("GET /echo/hello/there.html HTTP/1.1\r\nname=1\r\n\r\n");
  }

  std::string test_buffer;
  std::unique_ptr<Request> request;
};

TEST_F(RequestTest, EmptyRequest) { 
  runParse();
  // EXPECT_EQ(result, Request::result_type::indeterminate);
}

TEST_F(RequestTest, CorrectRequest) { 
  properRequest();
  // EXPECT_EQ(result, Request::result_type::good);
}

// Correct Handler + File Paths
TEST_F(RequestTest, CorrectPathsHandlerRequest) {
  properRequest();
  EXPECT_EQ(request->uri(), "/echo/hello/there.html");
  // EXPECT_EQ(result, Request::result_type::good);
}

TEST_F(RequestTest, IncorrectRequest) { 
  setUpRequest("asdfj<kl;");
  // EXPECT_EQ(result, Request::result_type::bad);
}
