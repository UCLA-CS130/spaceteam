#include <iostream>
#include <vector>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "config_parser.h"
#include "server.h"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_service& io_service, const char* filename)
    : acceptor_(io_service) {

  ServerInfo info;
  if (!getServerInfo(filename, &info)) {
    throw "Error parsing config file.";
  }
  printf("%s\n", info.ToString().c_str());
  port_ = info.port;
  static_path_to_root_ = info.static_path_to_root_;
  echo_path_to_root_ = info.echo_path_to_root_;

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
      Connection::create(acceptor_.get_io_service(), &echo_path_to_root_, &static_path_to_root_);

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
  
  for(unsigned i = 0; i < config.statements_.size(); i++) {
    std::string key = config.statements_[i]->tokens_[0];
    if(key == "port") {
      info->port = std::stoi(config.statements_[i]->tokens_[1]);
    } else if (key == "path") {
      if(config.statements_[i]->tokens_.size() != 3
          || !config.statements_[i]->child_block_) {
        return false;
      }
      // determine handler type
      if(config.statements_[i]->tokens_[2] == "StaticFileHandler") {
        if(config.statements_[i]->child_block_->statements_.size() != 0) {
          info->static_path_to_root_[config.statements_[i]->tokens_[1]] =
            config.statements_[i]->child_block_->statements_[0]->tokens_[1];
          } else {
            info->static_path_to_root_[config.statements_[i]->tokens_[1]] = "";
          }
      } else if(config.statements_[i]->tokens_[2] == "EchoHandler") {
        if(config.statements_[i]->child_block_->statements_.size() != 0) {
          info->echo_path_to_root_[config.statements_[i]->tokens_[1]] =
            config.statements_[i]->child_block_->statements_[0]->tokens_[1];
          } else {
            info->echo_path_to_root_[config.statements_[i]->tokens_[1]] = "";
          }
      } else {
        printf("Handler %s not recognized.", config.statements_[i]->tokens_[2].c_str());
        return false;
      }
    } else {
      printf ("Unexpected statment: %s %s;\n",
                config.statements_[i]->tokens_[0].c_str(),
                config.statements_[i]->tokens_[1].c_str());
      return false;
    }
  }
  return true;
}