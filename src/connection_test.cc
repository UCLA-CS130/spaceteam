// idea for tests taken from 
// https://github.com/UCLA-CS130/TBD/blob/master/connection_test.cc

#define TEST_CONNECTION

#include "gtest/gtest.h"
#include "connection.h"
#include "echo_handler.h"

class ConnectionTest : public ::testing::Test {
 protected:
  ConnectionTest() {
    injectTestMaps();
    connection_ = Connection::create(io_service_, &path_to_handler_);
  }
  
  // Inject the maps created before connection is initialized.
  void injectTestMaps() {
    path_to_handler_["/echo"] =  new EchoHandler();
  }

  boost::asio::io_service io_service_;
  std::map<std::string, RequestHandler*> path_to_handler_;
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
