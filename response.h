#ifndef RESPONSE_H
#define RESPONSE_H

#include <vector>
#include "header.h"

struct Response {
  int status;
  std::vector<header> headers;
  char *content;
};

#endif
