#include <iostream>
#include <vector>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <algorithm>

#include "config_parser.h"
#include "server_info.h"
#include "server.h"

using boost::asio::ip::tcp;

Server *Server::makeServer(boost::asio::io_service& io_service,
                           const char* config_file) {
  ServerInfo info;
  if (!getServerInfo(config_file, &info)) {
    return nullptr;  // error with config file
  }
  printf("%s\n", info.ToString().c_str());

  return new Server(io_service, info);

}

Server::Server(boost::asio::io_service& io_service,
               const ServerInfo info)
    : acceptor_(io_service) {
  
  port_ = info.port;
  path_to_info_ = info.path_to_info;

  tcp::endpoint endpoint(tcp::v6(), port_);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();

  start_accept();
}

// creates a socket and initiates asynchronous accept operation
// to wait for a new connection
void Server::start_accept() {
  Connection::pointer new_connection =
      Connection::create(acceptor_.get_io_service(),
                         &path_to_info_);

  acceptor_.async_accept(
      new_connection->socket(),
      boost::bind(
          &Server::handle_accept,
          this,
          new_connection,
          boost::asio::placeholders::error));
}

bool Server::handle_accept(Connection::pointer new_connection,
                           const boost::system::error_code& error) {
  if (error) {
    return false;
  }

  new_connection->start();
  start_accept();
  return true;
}

bool Server::getServerInfo(const char* file_name, ServerInfo* info) {
  NginxConfigParser parser;
  NginxConfig config;

  if (!parser.Parse(file_name, &config)) {
    return false;
  }
  
  // Keep track of port and path names to detect duplicates
  int port = -1;
  std::vector<std::string> path_names;

  for(size_t i = 0; i < config.statements_.size(); i++) {
    std::string key = config.statements_[i]->tokens_[0];
    if(key == "port") {
      if(port != -1) {
        return false; // found duplicate port
      }
      port = std::stoi(config.statements_[i]->tokens_[1]);

    } else if (key == "path") {
      if(config.statements_[i]->tokens_.size() != 3
          || !config.statements_[i]->child_block_) {
        return false;
      }
      std::string name = config.statements_[i]->tokens_[1];
      if(std::find(path_names.begin(), path_names.end(), name) != path_names.end()) {
        return false; // found duplicate path
      }
      path_names.push_back(config.statements_[i]->tokens_[1]);
      PathInfo pathInfo;
      pathInfo.handler_id = config.statements_[i]->tokens_[2];
      pathInfo.config = config.statements_[i]->child_block_.get();
      info->path_to_info[name] = pathInfo;

    } else {
      printf ("Unexpected statment: %s %s;\n",
                config.statements_[i]->tokens_[0].c_str(),
                config.statements_[i]->tokens_[1].c_str());
      return false;
    }
  }
  info->port = port;
  return true;
}