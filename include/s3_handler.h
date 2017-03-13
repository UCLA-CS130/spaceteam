#ifndef S3_HANDLER_H
#define S3_HANDLER_H

#include <string>
#include "request.h"
#include "response.h"
#include "config_parser.h"
#include "request_handler.h"

class S3Handler : public RequestHandler {
 public:
  Status Init(const std::string& uri_prefix,
              const NginxConfig& config);
  Status HandleRequest(const Request& request,
                       Response* response);

 private:
  std::string GetS3Object(const std::string& key);
  
  std::string uri_prefix_;
  std::string bucket_;

  const std::string CONFIG_BUCKET_KEY_ = "bucket";
  const std::string CONTENT_TYPE_ = "Content-Type";
  const std::string TEXT_HTML_ = "text/html";
};

REGISTER_REQUEST_HANDLER(S3Handler);

#endif
