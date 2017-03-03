#include <string>
#include <istream>
#include <ostream>
#include <boost/asio.hpp>
#include "proxy_handler.h"

using boost::asio::ip::tcp;
RequestHandler::Status ProxyHandler::Init(const std::string& uri_prefix,
         const NginxConfig& config) {
	uri_prefix_ = uri_prefix;
	RequestHandler::ParseConfig(config);
	host_ = config_map_[CONFIG_HOST_KEY_];
	portno_ = config_map_[CONFIG_PORT_KEY_];

	return OK;
}

RequestHandler::Status ProxyHandler::HandleRequest(const Request& request,
		Response* response) {
    //This function is inspired by www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/example/http/client/sync_client.cpp
    std::string uri;
    if (request.uri() == uri_prefix_) {
    uri = "/" ;
    } else {
    uri = request.uri();
    }
    if (uri != uri_prefix_ 
        && uri.size() > uri_prefix_.size() 
        && uri.substr(0, uri_prefix_.size()) == uri_prefix_) {
      uri=  uri.substr(uri_prefix_.size());
    } 
  
    //Setting up connection to remote host,port
	  boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host_, portno_);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);
    
    //Creating the HTTP request and querrying
    std::string req = "GET " + uri + " HTTP/1.1\r\n";
    req = req + "Host: " + host_ + "\r\n";
    req = req + "Accept: */*\r\n";
    req= req + "Connection: close\r\n\r\n";
    boost::asio::write(socket, boost::asio::buffer(req, req.size()));

    //Parsing the response
    boost::asio::streambuf resp;
    boost::asio::read_until(socket, resp, "\r\n");
    std::istream response_stream(&resp);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    //Setting status code
    if(status_code==200) {
      response->SetStatus(Response::OK);
    }
    else if(status_code==302) {
      response->SetStatus(Response::REDIRECT);
    }
    else {
      response->SetStatus(Response::NOT_FOUND);
    }
    if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
        response->SetStatus(Response::INTERNAL_SERVER_ERROR);
        return ERROR;
    }
    
    //pasrsing response headers
    boost::asio::read_until(socket, resp, "\r\n\r\n");
    std::string currheader;
    while (std::getline(response_stream, currheader) && currheader != "\r") {
        int pos = currheader.find(":");
        std::string name = currheader.substr(0, pos);
        std::string value = currheader.substr(pos + 2);
        response->AddHeader(name, value);
    }
    
    //parsing response content
    boost::system::error_code error;
    while (boost::asio::read(socket, resp, error)) {
        std::ostringstream temp;
        temp << &resp;
        response->SetBody(temp.str());
        if (error == boost::asio::error::eof) {
            break;
        }
    }
    
    if (error != boost::asio::error::eof) {
      throw boost::system::system_error(error);
    }

return OK;

}
