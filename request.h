#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <vector>
#include "header.h"

struct Request {
  std::string method;
  // handler_path + file_path is path from request, to decide which handler to use
  // Example: /pizza/crust/cheese.jpg, 
  //          handler_path = "/pizza"
  //          file_path = "/crust/cheese.jpg"
  std::string handler_path; 
  std::string file_path;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<Header> headers;
  std::string raw_request; // the original request text
};

#endif
