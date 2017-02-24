// Modeled from parser at: http://www.boost.org/doc/libs/1_62_0/doc/html/boost_asio/example/cpp11/http/server/request_parser.hpp
// Implementation to parse HTTP requests

#include "request.h"

#include <iostream>

// Parse some data. The enum return value is good when a complete request has
// been parsed, bad if the data is invalid, indeterminate when more data is
// required. The InputIterator return value indicates how much of the input
// has been consumed.
std::unique_ptr<Request> Request::Parse(const std::string& raw_request) {
  std::unique_ptr<Request> parsed_request(new Request());
  for (char c : raw_request) {
    Request::result_type result = parsed_request->consume(c);
    parsed_request->parsed_status_ = result;
    // If there's a bad request, stop parsing.
    if (result == Request::result_type::bad) {
      break;
    }
  }
  return parsed_request;
}

Request::result_type Request::consume(char input) {
  raw_request_.push_back(input);

  switch (state_) {
    case method_start: {
      if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return bad;
      } else {
        state_ = method_inside;
        method_.push_back(input);
        return indeterminate;
      }
    }
    case method_inside: {
      if (input == ' ') {
        state_ = handler_path_start;
        return indeterminate;
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return bad;
      } else {
        method_.push_back(input);
        return indeterminate;
      }
    }
    case handler_path_start: {
      if (input == '/') {
        state_ = handler_path;
        handler_path_.push_back(input);
        uri_.push_back(input);
        return indeterminate;
      } else {
        return bad;
      }
    }
    case handler_path: {
      if (input == ' ') {
        state_ = http_version_h;
        return indeterminate;
      } else if (is_ctl(input)) {
        return bad;
      } else if (input == '/') {
        state_ = uri_inside;
        file_path_.push_back(input);
      } else {
        handler_path_.push_back(input);  
      }
      uri_.push_back(input);
      return indeterminate;
    }
    case uri_inside: {
      if (input == ' ') {
        state_ = http_version_h;
        return indeterminate;
      } else if (is_ctl(input)) {
        return bad;
      } else {
        uri_.push_back(input);
        file_path_.push_back(input);
        return indeterminate;
      }
    }
    case http_version_h: {
      if (input == 'H') {
        state_ = http_version_t_1;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_t_1: {
      if (input == 'T') {
        state_ = http_version_t_2;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_t_2: {
      if (input == 'T') {
        state_ = http_version_p;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_p: {
      if (input == 'P') {
        state_ = http_version_slash;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_slash: {
      if (input == '/') {
        http_version_major_ = 0;
        http_version_minor_ = 0;
        state_ = http_version_major_start;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_major_start: {
      if (is_digit(input)) {
        http_version_major_ = http_version_major_ * 10 + input - '0';
        version_.push_back(input);
        state_ = http_version_major;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_major: {
      if (input == '.') {
        version_.push_back(input);
        state_ = http_version_minor_start;
        return indeterminate;
      } else if (is_digit(input)) {
        http_version_major_ = http_version_major_ * 10 + input - '0';
        version_.push_back(input);
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_minor_start: {
      if (is_digit(input)) {
        http_version_minor_ = http_version_minor_ * 10 + input - '0';
        version_.push_back(input);
        state_ = http_version_minor;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case http_version_minor: {
      if (input == '\r') {
        state_ = expecting_newline_1;
        return indeterminate;
      } else if (is_digit(input)) {
        http_version_minor_ = http_version_minor_ * 10 + input - '0';
        version_.push_back(input);
        return indeterminate;
      } else {
        return bad;
      }
    }
    case expecting_newline_1: {
      if (input == '\n') {
        state_ = header_line_start;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case header_line_start: {
      if (input == '\r') {
        state_ = expecting_newline_3;
        return indeterminate;
      } else if (!headers_.empty() && (input == ' ' || input == '\t')) {
        state_ = header_lws;
        return indeterminate;
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return bad;
      } else {
        headers_.push_back(std::pair<std::string, std::string>());
        headers_.back().first.push_back(input);
        state_ = header_name;
        return indeterminate;
      }
    }
    case header_lws: {
      if (input == '\r') {
        state_ = expecting_newline_2;
        return indeterminate;
      } else if (input == ' ' || input == '\t') {
        return indeterminate;
      } else if (is_ctl(input)) {
        return bad;
      } else {
        state_ = header_value;
        headers_.back().second.push_back(input);
        return indeterminate;
      }
    }
    case header_name: {
      if (input == ':') {
        state_ = space_before_header_value;
        return indeterminate;
      } else if (!is_char(input) || is_ctl(input) || is_tspecial(input)) {
        return bad;
      } else {
        headers_.back().first.push_back(input);
        return indeterminate;
      }
    }
    case space_before_header_value: {
      if (input == ' ') {
        state_ = header_value;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case header_value: {
      if (input == '\r') {
        state_ = expecting_newline_2;
        return indeterminate;
      } else if (is_ctl(input)) {
        return bad;
      } else {
        headers_.back().second.push_back(input);
        return indeterminate;
      }
    }
    case expecting_newline_2: {
      if (input == '\n') {
        state_ = header_line_start;
        return indeterminate;
      } else {
        return bad;
      }
    }
    case expecting_newline_3: {
      if (input == '\n') {
        state_ = body_state;
        return good;
      } else {
        return bad;
      }
    }
    case body_state: {
      // place the rest of the text after headers into the body
      body_.push_back(input);
      return good;
    }
    default: {
      return bad;
    }
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
  return raw_request_;
}

std::string Request::method() const {
  return method_;
}

std::string Request::uri() const {
  return uri_;
}

std::string Request::version() const {
  return version_;
}

Request::Headers Request::headers() const {
  return headers_;
}

std::string Request::body() const {
  return body_;
}

Request::result_type Request::parsed_status() const {
  return parsed_status_;
}