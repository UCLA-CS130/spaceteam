#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <string>
#include <map>
#include <memory>

#include "config_parser.h"
#include "request_handler.h"

struct PathInfo {

	/* Name must match id in corresponding request handler */
  std::string handler_id;

  /* Contains the config for this particular handler */
  NginxConfig* config;

  /* Pointer to the actual handler object */
	RequestHandler* handler;

};

struct ServerInfo {
  int port;

  /* Maps url path to its config */
  std::map<std::string, PathInfo> path_to_info;

};

#endif
