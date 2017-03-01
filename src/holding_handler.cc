#include "holding_handler.h"

RequestHandler::Status HoldingHandler::Init(const std::string& uri_prefix,
                                                const NginxConfig& config) {
  return OK;
}

RequestHandler::Status HoldingHandler::HandleRequest(const Request& request,
                                                         Response* response) {
  std::cout << "Waiting on HoldingHandler" << std::endl;
  // Purposeful infinite loop to keep the connection alive
  while(true) {}

  return OK;
}
