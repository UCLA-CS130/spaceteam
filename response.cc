#include "response.h"

#include <iostream>

std::vector<boost::asio::const_buffer> Response::to_buffers() {
  const std::string crlf = "\r\n";
  std::vector<boost::asio::const_buffer> buffers;

  buffers.push_back(boost::asio::buffer(status_code_to_string(status)));
  for (std::size_t i = 0; i < headers.size(); ++i) {
    Header& h = headers[i];
    buffers.push_back(boost::asio::buffer(h.to_string()));
    buffers.push_back(boost::asio::buffer(crlf));
  }
  buffers.push_back(boost::asio::buffer(crlf));
  buffers.push_back(boost::asio::buffer(content));

  return buffers;
}

std::string Response::to_string() {
  std::string result = "";
  const std::string crlf = "\r\n";
  result += status_code_to_string(status);
  for (auto h : headers) {
    result += h.to_string();
    result += crlf;
  }
  result += crlf;
  result += content;
  
  return result;
}

std::string Response::status_code_to_string(int status_code) {
  switch (status_code) {
    case 200: {
      return "HTTP/1.1 200 OK\r\n";
    }
    default: {
      return "HTTP/1.1 500 Internal Server Error\r\n";
    }
  }
}
