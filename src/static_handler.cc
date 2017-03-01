#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "not_found_handler.h"
#include "static_handler.h"

RequestHandler::Status StaticHandler::Init(const std::string& uri_prefix,
                                           const NginxConfig& config) {
  uri_prefix_ = uri_prefix;
  RequestHandler::ParseConfig(config);
  root_ = config_map_[CONFIG_ROOT_KEY_];
  return OK;
}

// TODO: add request validation?
RequestHandler::Status StaticHandler::HandleRequest(const Request& request,
                                                    Response* response) {
  if (response == nullptr) {
    return ERROR;
  }

  // get relative path by taking the request URI, removing the StaticHandler's
  // URI prefix, and prepending the root path
  std::cout << "GOT URI:" + request.uri() << std::endl;
  std::string uri = request.uri();
  std::string relative_path_string;
  if (uri != uri_prefix_ 
      && uri.size() > uri_prefix_.size() 
      && uri.substr(0, uri_prefix_.size()) == uri_prefix_) {
    relative_path_string = root_ + uri.substr(uri_prefix_.size());
  } else {
    return ERROR;
  }
  boost::filesystem::path relative_path(relative_path_string);

  if (boost::filesystem::exists(relative_path)
      && boost::filesystem::is_regular_file(relative_path)) {
    // use absolute path to get rid of '..', symlinks, etc. in the path
    boost::filesystem::path absolute_path = boost::filesystem::canonical(
        relative_path);

    std::cout << "RELATIVE PATH" << std::endl;

    // get name of base directory
    std::string dir = boost::filesystem::canonical(
        boost::filesystem::path(root_)).filename().string();

    // if base directory is in absolute path (file is within the base directory)
    if (absolute_path.string().find(dir) != std::string::npos) {
      boost::filesystem::ifstream ifs(absolute_path, 
                                      std::ios::in | std::ios::binary);

      std::cout << "ABSOLUTE PATH" << std::endl;

      if (ifs) {
        // read file into response body
        std::ostringstream oss;
        oss << ifs.rdbuf();
        ifs.close();
        response->SetBody(oss.str());

        // set mime type
        std::string extension = absolute_path.extension().string();
        response->AddHeader(CONTENT_TYPE_, GetMimeType(extension));

        std::cout << "GOT IT" << std::endl;

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
