#ifndef PROXY_HANDLER_H
#define PROXY_HANDELR_H

#include <string>
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "request_handler.h"

class ProxyHandler : public RequestHandler {
public:
 	Status Init(const std::string& uri_prefix,
              const NginxConfig& config);
  	Status HandleRequest(const Request& request,
                       Response* response);
private:
	std::string uri_prefix_;
	std::string host_;
	std::string portno_;
	const std::string CONFIG_HOST_KEY_ = "host";
	const std::string CONFIG_PORT_KEY_ = "portno";
};

REGISTER_REQUEST_HANDLER(ProxyHandler);

#endif
