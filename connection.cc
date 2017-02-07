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

Connection::pointer Connection::create(boost::asio::io_service& io_service, 
                                       std::map<std::string, std::string>* input_echo_map, 
                                       std::map<std::string, std::string>* input_static_map) {
  return pointer(new Connection(io_service, input_echo_map, input_static_map));
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
    RequestHandler* request_handler = &echo_request_handler; 

    if (static_map->find(request.directory) != static_map->end()) {
      request_handler = &static_request_handler;
    } else if (echo_map->find(request.directory) == echo_map->end()) {
      // Directory for request is not found in either
      // For the sake of this implementation, continue with EchoRequestHandler
      std::cerr << "Couldn't find request directory in echo and static map" << std::endl;
    }

    Response response;
    request_handler->handle_request(request, response);
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
