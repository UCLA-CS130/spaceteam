#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <string>
#include <unordered_map>
#include "request_handler.h"

std::string get_mime_type(std::string extension);

void EchoRequestHandler::handle_request(const Request &request, 
                                        Response &response) {
  response.status = 200;
  Header content_type = {"Content-Type", "text/plain"};
  response.headers.push_back(content_type);
  response.content = request.raw_request;
}

void StaticRequestHandler::handle_request(const Request &request, 
                                          Response &response) {
  boost::filesystem::path relative_path(request.base_path + request.file_path);

  if (boost::filesystem::exists(relative_path)
      && boost::filesystem::is_regular_file(relative_path)) {
    boost::filesystem::path absolute_path = boost::filesystem::canonical(relative_path);

    // get name of base directory
    std::string dir = boost::filesystem::canonical(
        boost::filesystem::path(request.base_path)).filename().string();

    // if base directory is in absolute path (file is within the base directory)
    if (absolute_path.string().find(dir) != std::string::npos) {
      boost::filesystem::ifstream ifs(absolute_path, std::ios::in | std::ios::binary);
      if (ifs) {
        // read file into content
        std::ostringstream oss;
        oss << ifs.rdbuf();
        ifs.close();
        response.content = oss.str();

        // set mime type
        std::string extension = absolute_path.extension().string();
        Header mime_type = {"Content-Type", get_mime_type(extension)};
        response.headers.push_back(mime_type);

        response.status = 200;
      } else {
        response.status = 404;
      }
    } else {
      response.status = 404;
    }
  } else {
    response.status = 404;
  }
}

std::string get_mime_type(std::string extension) {
  std::unordered_map<std::string, std::string> mime_map;
  mime_map[".gif"] = "image/gif";
  mime_map[".htm"] = "text/html";
  mime_map[".html"] = "text/html";
  mime_map[".jpg"] = "image/jpeg";
  mime_map[".jpeg"] = "image/jpeg";
  mime_map[".png"] = "image/png";

  if (mime_map.count(extension) > 0) {
    return mime_map[extension];
  } else {
    return "text/plain";
  }
}
