#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <vector>

struct Request {
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<Header> headers;
  std::string raw_request; // the original request text
};

#endif
