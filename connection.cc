#include "connection.h"

using boost::asio::ip::tcp;

Connection::pointer Connection::create(boost::asio::io_service& io_service) {
  return pointer(new Connection(io_service));
}

tcp::socket& Connection::socket() {
  return socket_;
}

void Connection::start() {
  receive();
}

void Connection::receive() {
  auto self(shared_from_this());
  socket_.async_read_some(
      boost::asio::buffer(data_, max_request_length),
      [this, self](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
          send(length);
        }
      });
}

void Connection::send(std::size_t length) {
  auto self(shared_from_this());

  char response[max_response_length] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n";
  strcat(response, data_);
  data_[0] = '\0'; // clear out data_

  boost::asio::async_write(
      socket_, 
      boost::asio::buffer(response, strlen(response)),
      [this, self](boost::system::error_code ec, std::size_t /*length*/) {
        if (!ec) {
          receive();
        }
      });

  socket_.close();
}
