#include <string>

#include "status_handler.h"
#include "request_handler.h"
#include "config_parser.h"
#include "request.h"
#include "response.h"

RequestHandler::Status StatusHandler::Init(const std::string& uri_prefix,
                                                const NginxConfig& config) {
  return OK;
}

RequestHandler::Status StatusHandler::HandleRequest(const Request& request,
                                                         Response* response) {
	response->SetStatus(Response::OK);
	response->AddHeader(CONTENT_TYPE_, DEFAULT_CONTENT_TYPE_);
	ServerStatus* status = request.getServerStatus();
	if (status == nullptr) {
		return ERROR;
	}
	response->SetBody(status->ToHtml());

	return OK;
}
