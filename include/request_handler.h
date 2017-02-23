#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>
#include <unordered_map>
#include "request.h"
#include "response.h"
#include "config_parser.h"

// Represents the parent of all request handlers. Implementations should expect
// to be long lived and created at server construction.
class RequestHandler {
 public:
  enum Status {
    OK = 0,
    ERROR = 1
  };
  
  // Initializes the handler. Returns a response code indicating success or
  // failure condition.
  // uri_prefix is the value in the config file that this handler will run for.
  // config is the contents of the child block for this handler ONLY.
  virtual Status Init(const std::string& uri_prefix,
                      const NginxConfig& config) = 0;

  // Handles an HTTP request, and generates a response. Returns a response code
  // indicating success or failure condition. If ResponseCode is not OK, the
  // contents of the response object are undefined, and the server will return
  // HTTP code 500.
  virtual Status HandleRequest(const Request& request,
                               Response* response) = 0;

 protected:
  // Helper function to parse through handler config
  // returns map of names and their corresponding parameters
  using ConfigMap = std::unordered_map<std::string, std::string>;
  void ParseConfig(const NginxConfig& config);
  ConfigMap config_map_;
  
  const std::string CONTENT_TYPE = "Content-Type";
  const std::string TEXT_PLAIN = "text/plain";
};

#endif
