#include <iostream>
#include <boost/asio.hpp>
#include "server.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " config_file_path" << std::endl;
    return 1;
  }

  // Creates server object to accept incoming client connections
  boost::asio::io_service io_service;
  Server *server = Server::makeServer(io_service, argv[1]);
  if(server == nullptr) {
    std::cerr << "Error parsing config file." << std::endl;
    return 1;
  }
  io_service.run();
  return 0;
}
