#include <string>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "request_handler.h"

void RequestHandler::ParseConfig(const NginxConfig& config) {
  for (auto statement : config.statements_) {
    config_map_[statement->tokens_[0]] = statement->tokens_[1];
  }
}

RequestHandler::Status EchoHandler::Init(const std::string& uri_prefix,
                                         const NginxConfig& /*config*/) {
  uri_prefix_ = uri_prefix;
  return OK;
}

// todo: check if request.uri() matches uri_prefix_ ?
RequestHandler::Status EchoHandler::HandleRequest(const Request& request,
                                                  Response* response) {
  if (response == nullptr) {
    return ERROR;
  }

  response->SetStatus(Response::OK);
  response->AddHeader("Content-Type", "text/plain");
  response->SetBody(request.raw_request());

  return OK;
}

RequestHandler::Status StaticHandler::Init(const std::string& uri_prefix,
                                           const NginxConfig& config) {
  uri_prefix_ = uri_prefix;
  RequestHandler::ParseConfig(config);
  root_ = config_map_["root"];
  return OK;
}

// todo: add request validation
RequestHandler::Status StaticHandler::HandleRequest(const Request& request,
                                                    Response* response) {
  if (response == nullptr) {
    return ERROR;
  }

  // todo: change how we get relative_path
  std::string uri = request.uri();
  std::string relative_path_string;
  if (uri != uri_prefix_ 
      && uri.size() > uri_prefix_.size() 
      && uri.substr(0, uri_prefix_.size()) == uri_prefix_) {
    relative_path_string = root_ + uri.substr(uri_prefix_.size() - 1);
  } else {
    return ERROR;
  }

  boost::filesystem::path relative_path(relative_path_string);

  if (boost::filesystem::exists(relative_path)
      && boost::filesystem::is_regular_file(relative_path)) {
    boost::filesystem::path absolute_path = boost::filesystem::canonical(relative_path);

    // get name of base directory
    std::string dir = boost::filesystem::canonical(
        boost::filesystem::path(root_)).filename().string();

    // if base directory is in absolute path (file is within the base directory)
    if (absolute_path.string().find(dir) != std::string::npos) {
      boost::filesystem::ifstream ifs(absolute_path, std::ios::in | std::ios::binary);
      if (ifs) {
        // read file into response body
        std::ostringstream oss;
        oss << ifs.rdbuf();
        ifs.close();
        response->SetBody(oss.str());

        // set mime type
        std::string extension = absolute_path.extension().string();
        response->AddHeader("Content-Type", GetMimeType(extension));

        response->SetStatus(Response::OK);
        return OK;
      } 
    }
  }

  response->SetStatus(Response::NOT_FOUND);
  return NOT_FOUND;
}

std::string StaticHandler::GetMimeType(std::string extension) {
  std::unordered_map<std::string, std::string> mime_map;
  mime_map[".gif"] = "image/gif";
  mime_map[".htm"] = "text/html";
  mime_map[".html"] = "text/html";
  mime_map[".jpg"] = "image/jpeg";
  mime_map[".jpeg"] = "image/jpeg";
  mime_map[".png"] = "image/png";

  if (mime_map.count(extension) == 0) {
    return "text/plain";
  }
  return mime_map[extension];
}
