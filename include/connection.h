#ifndef CONNECTION_H
#define CONNECTION_H

#include <array>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <vector>
#include <map>

#include "request.h"
#include "response.h"
#include "request_handler.h"

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
  static pointer create(boost::asio::io_service& io_service, 
                        std::map<std::string, RequestHandler*>* input_path_to_handler);
  boost::asio::ip::tcp::socket& socket();
  void start();

  // Helper method, public for testing
  std::string ShortenUriPrefix(std::string path);

 private:
  Connection(boost::asio::io_service& io_service) : socket_(io_service) {}
  Connection(boost::asio::io_service& io_service, 
             std::map<std::string, RequestHandler*>* input_path_to_handler)
             : socket_(io_service), 
               path_to_handler_(input_path_to_handler) {}
  void do_read();
  bool handle_read(const boost::system::error_code& error, 
                   std::size_t bytes_transferred);
  void do_write(Response &response);
  bool handle_write(const boost::system::error_code& error,
                    std::size_t bytes_transferred);

  std::array<char, BUFFER_SIZE> buffer_;
  boost::asio::ip::tcp::socket socket_;
  // Maps given by server.cc to keep track of url paths to info
  std::map<std::string, RequestHandler*>* path_to_handler_;

  // Allows NotFoundHandler to be used
  const std::string DEFAULT_STRING = "default";

  // allow tests to access private members
  #ifdef TEST_CONNECTION
  FRIEND_TEST(ConnectionTest, HandleReadSuccess);
  FRIEND_TEST(ConnectionTest, HandleReadFailure);
  FRIEND_TEST(ConnectionTest, HandleWriteSuccess);
  FRIEND_TEST(ConnectionTest, HandleWriteFailure);
  #endif
};

#endif
