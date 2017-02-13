#ifndef HEADER_H
#define HEADER_H

#include <string>

struct Header {
  std::string name;
  std::string value;

  std::string to_string();
};

#endif
