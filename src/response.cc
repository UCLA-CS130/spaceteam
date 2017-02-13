#include "response.h"

std::string status_code_to_string(int status_code);

std::string Response::to_string() {
  std::string result = "";
  const std::string crlf = "\r\n";
  result += status_code_to_string(status) + crlf;
  for (auto h : headers) {
    result += h.to_string();
    result += crlf;
  }
  result += crlf;
  result += content;

  return result;
}

// helper method for Response::to_string()
std::string status_code_to_string(int status_code) {
  switch (status_code) {
    case 200: {
      return "HTTP/1.1 200 OK";
    }
    case 404: {
      return "HTTP/1.1 404 Not Found";
    }
    default: {
      return "HTTP/1.1 500 Internal Server Error";
    }
  }
}
