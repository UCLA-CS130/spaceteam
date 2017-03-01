#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <algorithm>

#include "config_parser.h"
#include "server.h"
#include "request_handler.h"

using boost::asio::ip::tcp;

Server *Server::makeServer(boost::asio::io_service& io_service,
                           const char* config_file) {
  ServerInfo info;
  if (!getServerInfo(config_file, &info)) {
    return nullptr;  // error with config file
  }

  return new Server(io_service, info);
}

Server::Server(boost::asio::io_service& io_service,
               const ServerInfo info)
    : acceptor_(io_service) {
  
  port_ = info.port;
  uri_prefix_to_handler_ = info.uri_prefix_to_handler;
  handler_name_to_prefixes_ = info.handler_name_to_prefixes;

  server_status_.AddHandlerInfo(handler_name_to_prefixes_);

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
                         &uri_prefix_to_handler_,
                         &server_status_);

  acceptor_.async_accept(
      new_connection->socket(),
      boost::bind(
          &Server::handle_accept,
          this,
          new_connection,
          boost::asio::placeholders::error));
}

// Run the server in threads
void Server::run() {
  // Modeled after team AAAAA from class, which was shown in class code reviews
  
  // List of shared pointers to each thread
  // Boost library does not support unique pointers for threads
  std::vector<boost::shared_ptr<boost::thread>> list_of_threads;
  for (std::size_t i = 0; i < NUM_OF_THREADS_; i++) {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
        boost::bind(&boost::asio::io_service::run, &acceptor_.get_io_service())));
    list_of_threads.push_back(thread);
  }

  // Wait for threads to exit before finishing
  for (std::size_t i = 0; i < list_of_threads.size(); i++) {
    list_of_threads[i]->join();
  }
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

  for (size_t i = 0; i < config.statements_.size(); i++) {
    std::string key = config.statements_[i]->tokens_[0];
    if (key == "port") {
      if (port != -1) {
        return false; // found duplicate port
      }
      port = std::stoi(config.statements_[i]->tokens_[1]);

    } else if (key == "path") {
      if (config.statements_[i]->tokens_.size() != 3
          || !config.statements_[i]->child_block_) {
        return false; // wrong format
      }
      std::string name = config.statements_[i]->tokens_[1];
      if (std::find(path_names.begin(), path_names.end(), name) != path_names.end()) {
        return false; // found duplicate path
      }
      path_names.push_back(config.statements_[i]->tokens_[1]);
      std::string handler_id = config.statements_[i]->tokens_[2];
      NginxConfig* handler_config = config.statements_[i]->child_block_.get();
      RequestHandler* handler = RequestHandler::CreateByName(handler_id.c_str());
      if(handler == nullptr) {
        return false; // Handler not found
      }
      handler->Init(name, *handler_config);
      info->uri_prefix_to_handler[name] = handler;
      info->handler_name_to_prefixes[handler_id].push_back(name);

    } else if (key == "default") {
      if (config.statements_[i]->tokens_.size() != 2
          || !config.statements_[i]->child_block_) {
        return false; // wrong format
      }
      std::string handler_id = config.statements_[i]->tokens_[1];
      // NginxConfig* handler_config = config.statements_[i]->child_block_.get();
      RequestHandler* handler = RequestHandler::CreateByName(handler_id.c_str());
      std::string path = "default";
      handler->Init(path.c_str(), *config.statements_[i]->child_block_);
      info->uri_prefix_to_handler["default"] = handler;
      info->handler_name_to_prefixes[handler_id].push_back("default");

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
