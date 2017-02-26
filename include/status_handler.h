#ifndef STATUS_HANDLER_H
#define STATUS_HANDLER_H

#include <string>
#include <map>

#include "request_handler.h"
#include "config_parser.h"
#include "request.h"
#include "response.h"

class StatusHandler : public RequestHandler {
public:
	Status Init(const std::string& uri_prefix, const NginxConfig& config);
  Status HandleRequest(const Request& request, Response* response);

private:
  const std::string CONTENT_TYPE_ = "Content-Type";
	const std::string DEFAULT_CONTENT_TYPE_ = "text/html";
};

REGISTER_REQUEST_HANDLER(StatusHandler);

#endif
