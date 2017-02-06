#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "request.h"
#include "response.h"

// base class for request handler
class RequestHandler {
 public:
  RequestHandler() {}
  virtual ~RequestHandler() {}
  virtual void handle_request(const Request &request, Response &response) = 0;
};

class EchoRequestHandler : public RequestHandler {
 public:
  EchoRequestHandler() {}
  ~EchoRequestHandler() {}
  void handle_request(const Request &request, Response &response);
};

class StaticRequestHandler : public RequestHandler {
 public:
  StaticRequestHandler() {}
  ~StaticRequestHandler() {}
  void handle_request(const Request &request, Response &response);
};

#endif
