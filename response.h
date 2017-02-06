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

  std::vector<boost::asio::const_buffer> to_buffers();
  std::string status_code_to_string(int status_code);
};

#endif
