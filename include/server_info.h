#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <string>
#include <map>
#include <memory>

#include "config_parser.h"

struct PathInfo {
  /* Name must match id in corresponding request handler */
  std::string handler_id;

  /* Contains the config for this particular handler */
  NginxConfig* config;

};

struct ServerInfo {
  int port;

  /* Maps url path to its config */
  std::map<std::string, PathInfo> path_to_info;

  std::string ToString();
};

// Helper function to parse through handler config
// returns map of names and their corresponding parameters
// std::map<std::string, std::string> parseHandlerConfig(NginxConfig* config) {
// 	std::map<std::string, std::string> name_to_param;
// 	for(size_t j = 0; j < config->statements_.size(); j++) {
// 	  name_to_param[config->statements_[j]->tokens_[0]]
// 	    = config->statements_[j]->tokens_[1];
//   }
//   return name_to_param;
// }
#endif
