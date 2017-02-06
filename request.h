#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <vector>

struct Request {
  std::string method_;
  std::string uri_;
  int http_version_major;
  int http_version_minor;
  std::vector<header> headers_;
  std::string raw_request_; // the original request text
};

#endif
