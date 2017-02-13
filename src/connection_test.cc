// idea for tests taken from 
// https://github.com/UCLA-CS130/TBD/blob/master/connection_test.cc

#define TEST_CONNECTION

#include "gtest/gtest.h"
#include "connection.h"

class ConnectionTest : public ::testing::Test {
 protected:
  ConnectionTest() {
    injectTestMaps();
    connection_ = Connection::create(io_service_, &echo_map_, &static_map_);
  }
  
  // Inject the maps created before connection is initialized.
  void injectTestMaps() {
    echo_map_["/echo"] =  "";
    static_map_["/static"] = "static_files";
  }

  boost::asio::io_service io_service_;
  std::map<std::string, std::string> echo_map_;
  std::map<std::string, std::string> static_map_;
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
