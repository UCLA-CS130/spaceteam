#include <string>
#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "request_handler.h"

// todo: put each request handler into a separate file

const std::string CONTENT_TYPE = "Content-Type";
const std::string TEXT_PLAIN = "text/plain";
const std::string STATIC_CONFIG_ROOT_KEY = "root";

void RequestHandler::ParseConfig(const NginxConfig& config) {
  for (auto statement : config.statements_) {
    config_map_[statement->tokens_[0]] = statement->tokens_[1];
  }
}

RequestHandler::Status EchoHandler::Init(const std::string& /*uri_prefix*/,
                                         const NginxConfig& /*config*/) {
  return OK;
}

// todo: check if request.uri() matches uri_prefix_ ?
RequestHandler::Status EchoHandler::HandleRequest(const Request& request,
                                                  Response* response) {
  if (response == nullptr) {
    return ERROR;
  }

  response->SetStatus(Response::OK);
  response->AddHeader(CONTENT_TYPE, TEXT_PLAIN);
  response->SetBody(request.raw_request());

  return OK;
}

RequestHandler::Status StaticHandler::Init(const std::string& uri_prefix,
                                           const NginxConfig& config) {
  uri_prefix_ = uri_prefix;
  RequestHandler::ParseConfig(config);
  root_ = config_map_[STATIC_CONFIG_ROOT_KEY];
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
        response->AddHeader(CONTENT_TYPE, GetMimeType(extension));

        response->SetStatus(Response::OK);
        return OK;
      } 
    }
  }

  NotFoundHandler not_found_handler;
  not_found_handler.HandleRequest(request, response);
  
  return OK;
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

RequestHandler::Status NotFoundHandler::Init(const std::string& /*uri_prefix*/,
                                                const NginxConfig& /*config*/) {
  return OK;
}

RequestHandler::Status NotFoundHandler::HandleRequest(const Request& request,
                                                         Response* response) {
  response->SetStatus(Response::NOT_FOUND);
  response->AddHeader(CONTENT_TYPE, TEXT_PLAIN);
  response->SetBody("<!doctype html><html><body><h1>404 Not Found</h1></body></html>");

  return OK;
}
