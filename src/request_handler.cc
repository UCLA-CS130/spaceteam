#include "request_handler.h"

std::map<std::string, RequestHandler* (*)(void)>* request_handler_builders = nullptr;

RequestHandler* RequestHandler::CreateByName(const char* type) {
  const auto type_and_builder = request_handler_builders->find(type);
  if (type_and_builder == request_handler_builders->end()) {
    return nullptr;
  }
  return (*type_and_builder->second)();
}

void RequestHandler::ParseConfig(const NginxConfig& config) {
  for (auto statement : config.statements_) {
    config_map_[statement->tokens_[0]] = statement->tokens_[1];
  }
}
