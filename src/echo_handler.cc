#include "echo_handler.h"

RequestHandler::Status EchoHandler::Init(const std::string& /*uri_prefix*/,
                                         const NginxConfig& /*config*/) {
  return OK;
}

// todo: check if request.uri() matches uri_prefix_ ?
RequestHandler::Status EchoHandler::HandleRequest(const Request& request,
                                                  Response* response) {
  if (response == nullptr) {
    return ERROR;
  }

  response->SetStatus(Response::OK);
  response->AddHeader(CONTENT_TYPE, TEXT_PLAIN);
  response->SetBody(request.raw_request());

  return OK;
}
