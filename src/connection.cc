#include <iostream>
#include <boost/bind.hpp>
#include <map>

#include "connection.h"
#include "request_handler.h"
#include "response.h"
#include "server_info.h"

using boost::asio::ip::tcp;

Connection::~Connection() {
  socket_.close();
}

Connection::pointer Connection::create(boost::asio::io_service& io_service) {
  return pointer(new Connection(io_service));
}

Connection::pointer Connection::create(boost::asio::io_service& io_service, 
                                       std::map<std::string, PathInfo>* input_path_to_info) {
  return pointer(new Connection(io_service, input_path_to_info));
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
  // TODO: use the parser to parse the request.
  // std::tie(result, std::ignore) = request_parser_.parse(
  //     request, buffer_.data(), buffer_.data() + bytes_transferred);
  // The line under this is meant to keep it compiling.
  // result = Request::good;

  // if (result == Request::good) {
    // You'll need this: RequestHandler request_handler;
    // Need to iterate through handler_id possibilities by longest prefix
    // So... maybe add a for loop or something?
    // std::string handler_id = request.uri(); // recommend changing name to handler_id in Request struct
    // for(auto const &e : *path_to_info_) {
      // if(e.first == handler_id) {
        // Instantiate a handler --> see https://piazza.com/class/iwy6115h8ce37q?cid=68
        // request_handler = RequestHandler::???(handler_id);
        // break;
      // }
    // }
    // detect if request_handler never got initialized
    // if so set it to a default handler, or maybe just return false ?
    Response response;
    // request_handler->handle_request(request, response);

    /* Also note you can process the handler specific config
       into a nice map using parseHandlerConfig();
       which is defined in server_info.h but is throwing compile error heh */
    
  //   do_write(response);

  // }
  // else {
  //   do_read();
  // }

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
