#include "response.h"

void Response::SetStatus(const ResponseCode response_code) {
  switch (response_code) {
    case OK: {
      status_ = "HTTP/1.1 200 OK";
      break;
    }
    case NOT_FOUND: {
      status_ = "HTTP/1.1 404 Not Found";
      break;
    }
    case INTERNAL_SERVER_ERROR:
    default: {
      status_ = "HTTP/1.1 500 Internal Server Error";
      break;
    }
  }
}
  
void Response::AddHeader(const std::string& header_name, 
                         const std::string& header_value) {
  headers_.push_back(std::make_pair(header_name, header_value));
}

void Response::SetBody(const std::string& body) {
  body_ = body;
}

std::string Response::ToString() {
  const std::string crlf = "\r\n";
  
  std::string result = status_ + crlf;
  for (auto header : headers_) {
    result += header.first + ": " + header.second + crlf;
  }
  result += crlf;
  result += body_;

  return result;
}