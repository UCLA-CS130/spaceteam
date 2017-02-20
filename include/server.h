#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <vector>

#include "connection.h"
#include "server_info.h"
#include "request_handler.h"

class Server {
 public:
  // Factory method
  static Server *makeServer(boost::asio::io_service& io_service,
                            const char* config_file);

  // Parses config file and returns it in the ServerInfo struct
  // Public for testing
  static bool getServerInfo(const char* file_name, ServerInfo* info);

 private:
  Server(boost::asio::io_service& io_service,
         const ServerInfo info);
  void start_accept();
  bool handle_accept(Connection::pointer new_connection,
                     const boost::system::error_code& error);

  boost::asio::ip::tcp::acceptor acceptor_;
  int port_;
  /* Maps url path to its info */
  std::map<std::string, PathInfo> path_to_info_;
};

#endif
