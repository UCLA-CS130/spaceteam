#include "request_handler.h"

void RequestHandler::ParseConfig(const NginxConfig& config) {
  for (auto statement : config.statements_) {
    config_map_[statement->tokens_[0]] = statement->tokens_[1];
  }
}
