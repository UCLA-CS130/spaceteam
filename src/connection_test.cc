// idea for tests taken from 
// https://github.com/UCLA-CS130/TBD/blob/master/connection_test.cc

#define TEST_CONNECTION

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "connection.h"
#include "echo_handler.h"
#include "server_status.h"

class MockServerStatus : public ServerStatus {

};

class ConnectionTest : public ::testing::Test {
 protected:
  ConnectionTest() {
    injectTestMaps();
    MockServerStatus server_status;
    connection_ = Connection::create(io_service_,
                                     &uri_prefix_to_handler_,
                                     &server_status);
  }
  
  // Inject the maps created before connection is initialized.
  void injectTestMaps() {
    uri_prefix_to_handler_["/echo"] =  &echo_handler;
  }

  boost::asio::io_service io_service_;
  std::map<std::string, RequestHandler*> uri_prefix_to_handler_;
  EchoHandler echo_handler;
  Connection::pointer connection_;
};

TEST_F(ConnectionTest, ShortenUriPrefixNoSlash) {
  EXPECT_EQ(connection_->ShortenUriPrefix("sandwich"), "");
}

TEST_F(ConnectionTest, ShortenUriPrefixSlash) {
  EXPECT_EQ(connection_->ShortenUriPrefix("/sandwich"), "/");
}

TEST_F(ConnectionTest, ShortenUriPrefixCorrect) {
  EXPECT_EQ(connection_->ShortenUriPrefix("/sandwich/cheese.html"), "/sandwich");
}

TEST_F(ConnectionTest, HandleReadSuccess) {
  boost::system::error_code ec = 
      boost::system::errc::make_error_code(boost::system::errc::success);
  EXPECT_TRUE(connection_->handle_read(ec, 0));
}

TEST_F(ConnectionTest, HandleReadFailure) {
  boost::system::error_code ec = 
      boost::system::errc::make_error_code(boost::system::errc::connection_refused);
  EXPECT_FALSE(connection_->handle_read(ec, 0));
}

TEST_F(ConnectionTest, HandleWriteSuccess) {
  boost::system::error_code ec = 
      boost::system::errc::make_error_code(boost::system::errc::success);
  EXPECT_TRUE(connection_->handle_write(ec, 0));
}

TEST_F(ConnectionTest, HandleWriteFailure) {
  boost::system::error_code ec = 
      boost::system::errc::make_error_code(boost::system::errc::connection_refused);
  EXPECT_FALSE(connection_->handle_write(ec, 0));
}
