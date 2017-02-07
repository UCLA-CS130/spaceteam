#ifndef CONNECTION_H
#define CONNECTION_H

#include <array>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "request_parser.h"
#include "response.h"

// only need gtest_prod.h when testing
#ifdef TEST_CONNECTION
#include "gtest/gtest_prod.h"
#endif

#define BUFFER_SIZE 1024 // value chosen arbitrarily

// shared_ptr and enable_shared_from_this keeps the Connection alive
// as long as there's an operation referring to it.
class Connection
    : public boost::enable_shared_from_this<Connection> {
 public:
  ~Connection();
  typedef boost::shared_ptr<Connection> pointer;
  static pointer create(boost::asio::io_service& io_service);
  boost::asio::ip::tcp::socket& socket();
  void start();

 private:
  Connection(boost::asio::io_service& io_service) : socket_(io_service) {}
  void do_read();
  bool handle_read(const boost::system::error_code& error, 
                   std::size_t bytes_transferred);
  void do_write(Response &response);
  bool handle_write(const boost::system::error_code& error,
                    std::size_t bytes_transferred);

  std::array<char, BUFFER_SIZE> buffer_;
  boost::asio::ip::tcp::socket socket_;
  RequestParser request_parser_;

  // allow tests to access private members
  #ifdef TEST_CONNECTION
  FRIEND_TEST(ConnectionTest, HandleReadSuccess);
  FRIEND_TEST(ConnectionTest, HandleReadFailure);
  FRIEND_TEST(ConnectionTest, HandleWriteSuccess);
  FRIEND_TEST(ConnectionTest, HandleWriteFailure);
  #endif
};

#endif
