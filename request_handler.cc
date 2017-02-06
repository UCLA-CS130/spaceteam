#include "request_handler.h"

void EchoRequestHandler::handle_request(const Request &request, 
                                        Response &response) {
  response.status = 200;
  Header content_type = {"Content-Type", "text/plain"};
  response.content = request.raw_request;
}

void StaticRequestHandler::handle_request(const Request &request, 
                                          Response &response) {

}
