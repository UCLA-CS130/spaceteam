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

  // TODO: tentative if this is good practice for sending the string
  // std::string buffer_string = std::string(buffer_);
  std::string buffer_string;
  std::copy(buffer_.begin(), buffer_.begin()+bytes_transferred, std::back_inserter(buffer_string));

  // RequestParser will insert data into the request struct
  std::unique_ptr<Request> request = Request::Parse(buffer_string);

  // TODO: Decide if we want to check "status" to see if it parsed correctly...
  // If so, we may need to make the enum result_type public or play with 0, 1, 2 values

  // TODO: problem - can't use an abstract handler... maybe default to echo handler? 
  // RequestHandler request_handler;

  std::string handler_uri_prefix = request->uri();
    
  // Need to iterate through handler_id possibilities by longest prefix
  // TODO: Decide if we want to limit handler_id to "/" or ""
  while (handler_uri_prefix != "") {
    // Check the map to see if it holds handler_id
    if (path_to_info_->count(handler_uri_prefix) > 0) {
      PathInfo path_info = found_info_iterator->second;
      // TODO: Set the request handler accordingly.
      // request_handler = ????????????
      break;
    } else {
      // map does NOT contain handler id.. so reduce string.
      handler_uri_prefix = ShortenUriPrefix(handler_uri_prefix);
    }
  }

  // check if it was done or not

  // detect if request_handler never got initialized
  // if so set it to a default handler, or maybe just return false ?
  Response response;
  // request_handler->handle_request(request, response);

    /* Also note you can process the handler specific config
       into a nice map using parseHandlerConfig();
       which is defined in server_info.h but is throwing compile error heh */
    
    do_write(response);

  // if the request handler is not found, do_read
  // }
  // else {
  //   do_read();
  // }

  return true; // success
}

std::string Connection::ShortenUriPrefix(std::string uri_prefix) {
  // Reached last possible path.
  if (uri_prefix == "/") {
    return "";
  }

  // Reverse the path to find the first instance of '/'
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
