#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "server.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_service& io_service, int port)
    : acceptor_(io_service, tcp::endpoint(tcp::v6(), port)) {
  start_accept();
}

// creates a socket and initiates asynchronous accept operation
// to wait for a new connection
void Server::start_accept() {
  Connection::pointer new_connection =
      Connection::create(acceptor_.get_io_service());

  acceptor_.async_accept(new_connection->socket(),
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
