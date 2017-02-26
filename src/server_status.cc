#include <vector>

#include "server_status.h"
#include "response.h"

ServerStatus::ServerStatus() {
  num_requests_ = 0;
}

void ServerStatus::UpdateStatus(std::string uri_prefix, Response::ResponseCode status) {
  unsigned ustatus = ResponseCodeToString(status);
  if (!server_status_.count(uri_prefix)
        || !server_status_[uri_prefix].count(ustatus)) {
    server_status_[uri_prefix][ustatus] = 1;
  } else {
    server_status_[uri_prefix][ustatus]++;
  }
  num_requests_++;
}

unsigned ServerStatus::ResponseCodeToString(Response::ResponseCode code) {
  switch(code) {
    case Response::ResponseCode::OK: return 200;
    case Response::ResponseCode::NOT_FOUND: return 404;
    case Response::ResponseCode::INTERNAL_SERVER_ERROR: return 500;
    default: return 500;
  }
}

std::string ServerStatus::ToHtml() {
  std::string body = "<!doctype html><html><body>";
  body += "<h1>Server Status</h1>";
  body += "<h3>Handler Info</h3>";
  for (auto e : handler_name_to_prefixes_) {
    body += "<h5>URI prefixes for " + e.first + "</h5>";
    body += "<ul style=\"list-style-type:none\">";
    for (auto prefix : e.second) {
      body += "<li>" + prefix + "</li>";
    }
    body += "</ul></br>";
  }

  body += "</br>";
  body += "<h2>Total Requests: " + std::to_string(num_requests_) + "</h2>";
  for (auto e : server_status_) {
    body += "<h5>Number of requests for \"" + e.first + "\"</h5>";
    body += "<ul style=\"list-style-type:none\">";
    for (auto f : e.second) {
      body += "<li>" + std::to_string(f.first) + ": " + std::to_string(f.second) + "</li>";
    }
    body += "</ul>";
  }

  body += "</body></html>";
  
  return body;


  std::string status = "Total Requests: " + std::to_string(num_requests_) + '\n';
  for (auto e : server_status_) {
    status += "Prefix: " + e.first + '\n';
    for (auto f : e.second) {
      status += '\t' + std::to_string(f.first) + ' ' + std::to_string(f.second) + '\n';
    }
  }
  return status;
}

std::string ServerStatus::ToString() {
  std::string status = "Total Requests: " + std::to_string(num_requests_) + '\n';
  for (auto e : server_status_) {
    status += "Prefix: " + e.first + '\n';
    for(auto f : e.second) {
      status += '\t' + std::to_string(f.first) + ' ' + std::to_string(f.second) + '\n';
    }
  }
  return status;
}

void ServerStatus::AddHandlerInfo(std::map<std::string, std::vector<std::string>> info) {
  handler_name_to_prefixes_ = info;
}