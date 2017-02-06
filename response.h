#ifndef RESPONSE_H
#define RESPONSE_H

#include <vector>
#include "header.h"

struct Response {
  int status;
  std::vector<Header> headers;
  char *content;
};

#endif
