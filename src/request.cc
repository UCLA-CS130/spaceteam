// Modeled from parser at: http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/example/cpp11/http/server/request_parser.hpp
// Implementation to parse HTTP requests

#include "request.h"

// Parse some data. The enum return value is good when a complete request has
// been parsed, bad if the data is invalid, indeterminate when more data is
// required. The InputIterator return value indicates how much of the input
// has been consumed.
std::unique_ptr<Request> Request::Parse(const std::string& raw_request) {
  std::unique_ptr<Request> parsed_request(new Request());
  for (char c : raw_request) {
    parsed_request->consume(c);
  }
  return parsed_request;
}

Request::result_type Request::consume(char input) {
  m_raw_request.push_back(input);

  switch (m_state) {
    case method_start:
      if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return bad;
      } else {
        m_state = method_inside;
        m_method.push_back(input);
        return indeterminate;
      }
    case method_inside:
      if (input == ' ') {
        m_state = handler_path_start;
        return indeterminate;
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return bad;
      } else {
        m_method.push_back(input);
        return indeterminate;
      }
    case handler_path_start:
      if (input == '/') {
        m_state = handler_path;
        m_handler_path.push_back(input);
        m_uri.push_back(input);
        return indeterminate;
      } else {
        return bad;
      }
    case handler_path:
      if (input == ' ') {
        m_state = http_version_h;
        return indeterminate;
      } else if (is_ctl(input)) {
        return bad;
      } else if (input == '/') {
        m_state = uri_inside;
        m_file_path.push_back(input);
      } else {
        m_handler_path.push_back(input);  
      }
      m_uri.push_back(input);
      return indeterminate;
    case uri_inside:
      if (input == ' ') {
        m_state = http_version_h;
        return indeterminate;
      } else if (is_ctl(input)) {
        return bad;
      } else {
        m_uri.push_back(input);
        m_file_path.push_back(input);
        return indeterminate;
      }
    case http_version_h:
      if (input == 'H') {
        m_state = http_version_t_1;
        return indeterminate;
      } else {
        return bad;
      }
    case http_version_t_1:
      if (input == 'T') {
        m_state = http_version_t_2;
        return indeterminate;
      } else {
        return bad;
      }
    case http_version_t_2:
      if (input == 'T') {
        m_state = http_version_p;
        return indeterminate;
      } else {
        return bad;
      }
    case http_version_p:
      if (input == 'P') {
        m_state = http_version_slash;
        return indeterminate;
      } else {
        return bad;
      }
    case http_version_slash:
      if (input == '/') {
        m_http_version_major = 0;
        m_http_version_minor = 0;
        m_state = http_version_major_start;
        return indeterminate;
      } else {
        return bad;
      }
    case http_version_major_start:
      if (is_digit(input)) {
        m_http_version_major = m_http_version_major * 10 + input - '0';
        m_state = http_version_major;
        return indeterminate;
      } else {
        return bad;
      }
    case http_version_major:
      if (input == '.') {
        m_state = http_version_minor_start;
        return indeterminate;
      } else if (is_digit(input)) {
        m_http_version_major = m_http_version_major * 10 + input - '0';
        return indeterminate;
      } else {
        return bad;
      }
    case http_version_minor_start:
      if (is_digit(input)) {
        m_http_version_minor = m_http_version_minor * 10 + input - '0';
        m_state = http_version_minor;
        return indeterminate;
      } else {
        return bad;
      }
    case http_version_minor:
      if (input == '\r') {
        m_state = expecting_newline_1;
        return indeterminate;
      } else if (is_digit(input)) {
        m_http_version_minor = m_http_version_minor * 10 + input - '0';
        return indeterminate;
      } else {
        return bad;
      }
    case expecting_newline_1:
      if (input == '\n') {
        m_state = header_line_start;
        return indeterminate;
      } else {
        return bad;
      }
    case header_line_start:
      if (input == '\r') {
        m_state = expecting_newline_3;
        return indeterminate;
      } else if (!m_headers.empty() && (input == ' ' || input == '\t')) {
        m_state = header_lws;
        return indeterminate;
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return bad;
      } else {
        m_headers.push_back(std::pair<std::string, std::string>());
        m_headers.back().first.push_back(input);
        m_state = header_name;
        return indeterminate;
      }
    case header_lws:
      if (input == '\r') {
        m_state = expecting_newline_2;
        return indeterminate;
      } else if (input == ' ' || input == '\t') {
        return indeterminate;
      } else if (is_ctl(input)) {
        return bad;
      } else {
        m_state = header_value;
        m_headers.back().second.push_back(input);
        return indeterminate;
      }
    case header_name:
      if (input == ':') {
        m_state = space_before_header_value;
        return indeterminate;
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return bad;
      } else {
        m_headers.back().first.push_back(input);
        return indeterminate;
      }
    case space_before_header_value:
      if (input == ' ') {
        m_state = header_value;
        return indeterminate;
      } else {
        return bad;
      }
    case header_value:
      if (input == '\r') {
        m_state = expecting_newline_2;
        return indeterminate;
      } else if (is_ctl(input)) {
        return bad;
      } else {
        m_headers.back().second.push_back(input);
        return indeterminate;
      }
    case expecting_newline_2:
      if (input == '\n') {
        m_state = header_line_start;
        return indeterminate;
      } else {
        return bad;
      }
    case expecting_newline_3:
      return (input == '\n') ? good : bad;
    default:
      return bad;
  }
}

bool Request::is_char(int c) {
  return c >= 0 && c <= 127;
}

bool Request::is_ctl(int c) {
  return (c >= 0 && c <= 31) || (c == 127);
}

bool Request::is_tspecial(int c) {
  switch (c) {
    case '(': case ')': case '<': case '>': case '@':
    case ',': case ';': case ':': case '\\': case '"':
    case '/': case '[': case ']': case '?': case '=':
    case '{': case '}': case ' ': case '\t':
      return true;
    default:
      return false;
  }
}

bool Request::is_digit(int c) {
  return c >= '0' && c <= '9';
}

/* Getter methods for Request */

std::string Request::raw_request() const {
  return m_raw_request;
}

std::string Request::method() const {
  return m_method;
}

std::string Request::uri() const {
  return m_uri;
}

// TODO: Not quite sure what they mean by "version"
// Right now I just gather HTTP version major and minor
std::string Request::version() const {
  return m_version;
}

Request::Headers Request::headers() const {
  return m_headers;
}

// TODO: Mm... this shouldn't work.
// What do they mean by body? Body of the HTTP request .. ? 
std::string Request::body() const {
  return "";
}