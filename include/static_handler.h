#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H

#include <string>
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "request_handler.h"

class StaticHandler : public RequestHandler {
 public:
  Status Init(const std::string& uri_prefix,
              const NginxConfig& config);
  Status HandleRequest(const Request& request,
                       Response* response);

 private:
  std::string GetMimeType(std::string extension);
  
  std::string uri_prefix_;
  std::string root_;

  const std::string CONFIG_ROOT_KEY = "root";
};

#endif
