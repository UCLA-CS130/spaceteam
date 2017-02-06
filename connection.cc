#include <iostream>
#include <boost/bind.hpp>
#include "connection.h"
#include "request_handler.h"
#include "response.h"

using boost::asio::ip::tcp;

Connection::~Connection() {
  socket_.close();
}

Connection::pointer Connection::create(boost::asio::io_service& io_service) {
  return pointer(new Connection(io_service));
}

tcp::socket& Connection::socket() {
  return socket_;
}

void Connection::start() {
  do_read();
}

void Connection::do_read() {
  socket_.async_read_some(
      boost::asio::buffer(buffer_),
      boost::bind(
          &Connection::handle_read,
          shared_from_this(),
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}

bool Connection::handle_read(const boost::system::error_code& error, 
                             std::size_t bytes_transferred) {
  if (error) {
    return false; // error
  }

  // Since there's no error, we can parse the data.
  // Global RequestParser will insert data into request
  // Not sure if bytes transferred is how to get the current amount transferred 
  Request request;
  RequestParser::result_type result;
  std::tie(result, std::ignore) = request_parser_.parse(
      request, buffer_.data(), buffer_.data() + bytes_transferred);

  if (result == RequestParser::good) {
    // TODO: get the right request handler based on the request
    EchoRequestHandler request_handler;
    request.raw_request = std::string(buffer_.data());
    Response response;
    request_handler.handle_request(request, response);
    do_write(response);
  }
  else {
    do_read();
  }

  return true; // success
}

void Connection::do_write(Response &response) {
  boost::asio::async_write(
      socket_,
      response.to_buffers(),
      boost::bind(
          &Connection::handle_write, 
          shared_from_this(), 
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}

bool Connection::handle_write(const boost::system::error_code& error,
                              std::size_t /*bytes_transferred*/) {
  if (error) {
    return false; // error
  }

  return true; // success
}
