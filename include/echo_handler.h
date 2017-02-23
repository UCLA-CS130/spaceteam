#ifndef ECHO_HANDLER_H
#define ECHO_HANDLER_H

#include <string>
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "request_handler.h"

class EchoHandler : public RequestHandler {
 public:
  Status Init(const std::string& uri_prefix,
              const NginxConfig& config);
  Status HandleRequest(const Request& request,
                       Response* response);
};

#endif
