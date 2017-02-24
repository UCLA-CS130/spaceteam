#ifndef NOT_FOUND_HANDLER_H
#define NOT_FOUND_HANDLER_H

#include <string>
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "request_handler.h"

class NotFoundHandler : public RequestHandler {
 public:
  Status Init(const std::string& uri_prefix, const NginxConfig& config);
  Status HandleRequest(const Request& request, Response* response);

 private:
  const std::string CONTENT_TYPE_ = "Content-Type";
  const std::string DEFAULT_CONTENT_TYPE_ = "text/html";
};

REGISTER_REQUEST_HANDLER(NotFoundHandler);

#endif
