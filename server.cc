#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "server.h"
#include "config_parser.h"
#include <vector>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_service& io_service, const char* filename)
    : acceptor_(io_service) {
  
  NginxConfig config;
  NginxConfigParser parser;
  if(!parser.Parse(filename, &config)) {
    throw "Error parsing config file.";
  }

  port_ = std::stoi(config.statements_[0]->tokens_[1]);

  tcp::endpoint endpoint(tcp::v6(), port_);
  acceptor_.open(endpoint.protocol());
  acceptor_.bind(endpoint);
  acceptor_.listen();

  start_accept();
}

// creates a socket and initiates asynchronous accept operation
// to wait for a new connection
void Server::start_accept() {

  // acceptor_.listen();

  Connection::pointer new_connection =
      Connection::create(acceptor_.get_io_service());

  acceptor_.async_accept(
      new_connection->socket(),
      boost::bind(&Server::handle_accept, this, new_connection,
                  boost::asio::placeholders::error));
}

void Server::handle_accept(Connection::pointer new_connection,
                           const boost::system::error_code& error) {
  if (!error) {
    new_connection->start();
  }
  
  start_accept();
}
