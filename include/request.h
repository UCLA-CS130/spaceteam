#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <vector>
#include <memory>

#include "server_status.h"

// Represents an HTTP Request.
//
// Usage:
//   auto request = Request::Parse(raw_request);
class Request {
  public:

    static std::unique_ptr<Request> Parse(const std::string& raw_request);

    static std::unique_ptr<Request> Parse(const std::string& raw_request,
                                            ServerStatus *status);

    std::string raw_request() const;
    std::string method() const;
    std::string uri() const;
    std::string version() const;

    void setServerStatus(ServerStatus *status);
    ServerStatus *getServerStatus() const;

    using Headers = std::vector<std::pair<std::string, std::string>>;
    Headers headers() const;

    std::string body() const;

    // For testing successful parsing
    enum result_type { good, bad, indeterminate };
    result_type parsed_status() const;

  private:
    // Handle the next character of input.
    result_type consume(char input);

    std::string raw_request_;
    std::string method_;
    std::string uri_;
    std::string version_;
    std::string body_;
    Headers headers_;

    ServerStatus *server_status_;

    // added private variables for easier parsing
    std::string handler_path_;
    std::string file_path_;
    int http_version_major_;
    int http_version_minor_;
    result_type parsed_status_;

    // Check if a byte is an HTTP character.
    static bool is_char(int c);

    // Check if a byte is an HTTP control character.
    static bool is_ctl(int c);

    // Check if a byte is defined as an HTTP tspecial character.
    static bool is_tspecial(int c);

    // Check if a byte is a digit.
    static bool is_digit(int c);

    // The current state of the parser.
    enum state {
      method_start,
      method_inside,
      handler_path_start,
      handler_path,
      file_path,
      uri_inside,
      http_version_h,
      http_version_t_1,
      http_version_t_2,
      http_version_p,
      http_version_slash,
      http_version_major_start,
      http_version_major,
      http_version_minor_start,
      http_version_minor,
      expecting_newline_1,
      header_line_start,
      header_lws,
      header_name,
      space_before_header_value,
      header_value,
      expecting_newline_2,
      expecting_newline_3,
      body_state
    } state_;
};


#endif
