#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>

#include "connection.h"

struct ServerInfo;

class Server {
 public:
  // Factory method
  static Server *makeServer(boost::asio::io_service& io_service, const char* config_file);

  // Parses config file and returns it in the ServerInfo struct
  // Public for testing
  static bool getServerInfo(const char* file_name, ServerInfo* info);

 private:
  Server(boost::asio::io_service& io_service, const ServerInfo info);
  void start_accept();
  bool handle_accept(Connection::pointer new_connection,
                     const boost::system::error_code& error);

  boost::asio::ip::tcp::acceptor acceptor_;
  int port_;
  // maps relate request path to base path on file system
  std::map<std::string, std::string> echo_path_to_root_;
  std::map<std::string, std::string> static_path_to_root_;
};

struct ServerInfo {
  ServerInfo() : port(1880) {}
  int port;

  // maps relate request path to base path on file system
  std::map<std::string, std::string> echo_path_to_root_;
  std::map<std::string, std::string> static_path_to_root_;

  std::string ToString() {
    std::string output = "port: " + std::to_string(port) + "\n";
    output += "\nEcho Mappings:\n";
    for(auto const& entry : echo_path_to_root_) {
      output += entry.first + ": " + entry.second + "\n";
    }
    output += "\nStatic Mappings:\n";
    for(auto const& entry : static_path_to_root_) {
      output += entry.first + ": " + entry.second + "\n";
    }
    return output;
  }
};

#endif
