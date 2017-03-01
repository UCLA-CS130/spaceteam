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
	//This should not be hardcoded once we implement the appropriate config parsing
	//std::string host="www.ucla.edu";
	//std::string portno="80";

	std::string host;
	std::string portno;

	std::string getHost();
	std::string getPort();

	const std::string CONFIG_HOST_KEY_ = "host";
	const std::string CONFIG_PORT_KEY_ = "portno";
};

REGISTER_REQUEST_HANDLER(ProxyHandler);

#endif