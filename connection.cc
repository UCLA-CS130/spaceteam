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

  // RequestParser will insert data into the request struct
  Request request;
  RequestParser::result_type result;
  std::tie(result, std::ignore) = request_parser_.parse(
      request, buffer_.data(), buffer_.data() + bytes_transferred);

  if (result == RequestParser::good) {
    EchoRequestHandler echo_request_handler;
    StaticRequestHandler static_request_handler;

    // Default request handler
    RequestHandler request_handler = &echo_request_handler; 

    // In the future, we can make these constants.
    if (request.uri != "/echo" && request.uri != "/") {
      // If it's not default or echo, it's static.
      request_handler = &static_request_handler;
    }

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
      boost::asio::buffer(response.to_string()),
      boost::bind(
          &Connection::handle_write, 
          shared_from_this(), 
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
}

bool Connection::handle_write(const boost::system::error_code& error,
                              std::size_t bytes_transferred) {
  if (error) {
    return false; // error
  }

  return true; // success
}
