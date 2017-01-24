#include <iostream>
#include <boost/asio.hpp>
#include "server.h"

int main() {
  // Creates server object to accept incoming client connections
  try {
    boost::asio::io_service io_service;
    Server server(io_service, 8118);
    io_service.run();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
