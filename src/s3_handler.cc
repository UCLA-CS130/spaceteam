#include <sstream>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "not_found_handler.h"
#include "s3_handler.h"

RequestHandler::Status S3Handler::Init(const std::string& uri_prefix,
                                       const NginxConfig& config) {
  uri_prefix_ = uri_prefix;
  RequestHandler::ParseConfig(config);
  bucket_ = config_map_[CONFIG_BUCKET_KEY_];

  return OK;
}

RequestHandler::Status S3Handler::HandleRequest(const Request& request,
                                                Response* response) {
  if (response == nullptr) {
    return ERROR;
  }

  // get object key by taking the request URI and removing the S3Handler's URI
  // prefix
  std::string uri = request.uri();
  std::string key;
  if (uri != uri_prefix_ 
      && uri.size() > uri_prefix_.size() 
      && uri.substr(0, uri_prefix_.size()) == uri_prefix_) {
    key = uri.substr(uri_prefix_.size() + 1);
  } else {
    return ERROR;
  }

  response->SetBody(GetS3Object(key));
  response->AddHeader(CONTENT_TYPE_, TEXT_HTML_);
  response->SetStatus(Response::OK);

  return OK;
}

// http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
std::string S3Handler::GetS3Object(const std::string& key) {
  // system call to nodejs
  std::string cmd = "nodejs get-s3-object.js " + bucket_ + " " + key;
  std::array<char, 128> buffer;
  std::string result;
  std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (!feof(pipe.get())) {
    if (fgets(buffer.data(), 128, pipe.get()) != NULL) {
      result += buffer.data();
    }
  }
  return result;
}
