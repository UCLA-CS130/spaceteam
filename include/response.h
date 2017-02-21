#ifndef RESPONSE_H
#define RESPONSE_H

#include <vector>
#include <string>
#include <boost/asio.hpp>
#include "header.h"

struct Response {
  int status;
  std::vector<Header> headers;
  std::string content;

  std::string to_string();
};

#endif