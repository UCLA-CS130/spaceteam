#include <iostream>
#include <boost/asio.hpp>
#include "server.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " config_path" << std::endl;
    return 1;
  }

  // Creates server object to accept incoming client connections
  try {
    boost::asio::io_service io_service;
    Server server(io_service, argv[1]);
    io_service.run();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
