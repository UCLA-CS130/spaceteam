#include <iostream>
#include <boost/bind.hpp>
#include <map>

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
                                       std::map<std::string, RequestHandler*>* input_path_to_handler) {
  return pointer(new Connection(io_service, input_path_to_handler));
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

  std::string buffer_string;
  std::copy(buffer_.begin(), buffer_.begin()+bytes_transferred, std::back_inserter(buffer_string));

  // RequestParser will insert data into the request struct
  std::unique_ptr<Request> request = Request::Parse(buffer_string);

  std::string handler_uri_prefix = request->uri();
  // Holder for the request pointer
  RequestHandler* request_handler = nullptr;
    
  // Iterate through handler_id possibilities by longest prefix.
  while (handler_uri_prefix != "") {
    // Check the map to see if it holds handler_id.
    if (path_to_handler_->count(handler_uri_prefix) > 0) {
      request_handler = path_to_handler_->at(handler_uri_prefix);
      break;
    } else {
      // map does NOT contain handler id.. so reduce string.
      handler_uri_prefix = ShortenUriPrefix(handler_uri_prefix);
    }
  }

  // check if it was done or not
  if (request_handler != NULL) {
    Response response;
    request_handler->HandleRequest(*request, &response);
    std::cerr << "Found Request Handler matching this path." << std::endl;
    do_write(response);
  } else {
    // Request Handler not found.
    do_read();
    std::cerr << "Did not find any Request Handlers matching this path." << std::endl;
  }
  return true; // success
}

std::string Connection::ShortenUriPrefix(std::string uri_prefix) {
  // Reached last possible path.
  if (uri_prefix == "/") {
    return "";
  }

  std::string::size_type pos = uri_prefix.rfind('/');

  // If a slash is found, shorten to that slash.
  if (pos != std::string::npos) {
    std::string shortened = uri_prefix.substr(0, pos);
    if (shortened != "") {
      return shortened;
    } else {
      // Default to "/" if the string is at the highest folder.
      return "/";
    }
  } else {
    // Print error that there was text WITHOUT slash in the front.
    std::cout << "Path given doesn't have a slash in front of it." << std::endl;
    return "";
  }
}

void Connection::do_write(Response &response) {
  boost::asio::async_write(
      socket_,
      boost::asio::buffer(response.ToString()),
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
