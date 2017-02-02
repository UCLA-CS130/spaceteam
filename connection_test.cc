// idea for tests taken from 
// https://github.com/UCLA-CS130/TBD/blob/master/connection_test.cc

#include "gtest/gtest.h"
#include "connection.h"

class ConnectionTest : public ::testing::Test {
 protected:
  ConnectionTest() {
    connection_ = Connection::create(io_service_);
  }

  boost::asio::io_service io_service_;
  Connection::pointer connection_;
};

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
