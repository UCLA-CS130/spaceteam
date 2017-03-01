#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <map>
#include <vector>
#include <string>

#include "connection.h"
#include "request_handler.h"

struct ServerInfo {
  int port;

  /* Maps url path to its config */
  std::map<std::string, RequestHandler*> uri_prefix_to_handler;

  std::map<std::string, std::vector<std::string>> handler_name_to_prefixes;

};

class Server {
 public:
  // Factory method
  static Server *makeServer(boost::asio::io_service& io_service,
                            const char* config_file);

  // Parses config file and returns it in the ServerInfo struct
  // Public for testing
  static bool getServerInfo(const char* file_name, ServerInfo* info);

  // Runs the server on main.cpp
  void run();

 private:
  Server(boost::asio::io_service& io_service,
         const ServerInfo info);
  void start_accept();
  bool handle_accept(Connection::pointer new_connection,
                     const boost::system::error_code& error);

  boost::asio::ip::tcp::acceptor acceptor_;
  int port_;
  // Default is 5 threads, can be specified in config in the future.
  const std::size_t NUM_OF_THREADS_ = 5;

  std::map<std::string, RequestHandler*> uri_prefix_to_handler_;
  ServerStatus server_status_;
  // Info for ServerStatus
  std::map<std::string, std::vector<std::string>> handler_name_to_prefixes_;
};

#endif
