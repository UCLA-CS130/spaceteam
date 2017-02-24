#include "not_found_handler.h"

RequestHandler::Status NotFoundHandler::Init(const std::string& uri_prefix,
                                                const NginxConfig& config) {
  return OK;
}

RequestHandler::Status NotFoundHandler::HandleRequest(const Request& request,
                                                         Response* response) {
  response->SetStatus(Response::NOT_FOUND);
  response->AddHeader(CONTENT_TYPE_, DEFAULT_CONTENT_TYPE_);
  response->SetBody("<!doctype html><html><body><h1>404 Not Found</h1></body></html>");

  return OK;
}
