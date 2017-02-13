#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>

#include "connection.h"

class Server {
 public:
  Server(boost::asio::io_service& io_service, const char* filename);

 private:
  void start_accept();
  bool handle_accept(Connection::pointer new_connection,
                     const boost::system::error_code& error);

  boost::asio::ip::tcp::acceptor acceptor_;
  int port_;
  // maps relate request path to base path on file system
  std::map<std::string, std::string> echo_path_to_root_;
  std::map<std::string, std::string> static_path_to_root_;
};

#endif
