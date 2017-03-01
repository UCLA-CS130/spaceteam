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
	//host = RequestHandler::getHost();
	//portno = RequestHandler::getPort();
	//TODO: Implement above function
	return OK;
}

RequestHandler::Status ProxyHandler::HandleRequest(const Request& request,
		Response* response) {
	if(response==nullptr)
		return ERROR;

	std::string uri = request.uri();
  	std::string relative_path_string;
  	if (uri != uri_prefix_ 
      && uri.size() > uri_prefix_.size() 
      && uri.substr(0, uri_prefix_.size()) == uri_prefix_
      && uri_prefix_.compare("/")!=0) {
    relative_path_string = uri.substr(uri_prefix_.size());
  } else {
    relative_path_string = uri;
  }
boost::asio::io_service io_service_;
tcp::resolver resolver(io_service_);
tcp::resolver::query query(host, "http");
tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

tcp::socket socket(io_service_);
boost::asio::connect(socket, endpoint_iterator);

boost::asio::streambuf req;
std::ostream request_stream(&req);
request_stream << "GET "<< relative_path_string<< " HTTP/1.1\r\n";
request_stream << "Host: "<< host<<":"<<portno<< "\r\n";
request_stream<<"Accept: */*\r\n";
request_stream<<"Connection: close \r\n\r\n";

boost::asio::write(socket, req);

boost::asio::streambuf resp;
boost::asio::read_until(socket, resp, "\r\n");

	std::istream response_stream(&resp);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);

if(status_code==302)
response->SetStatus(Response::REDIRECT);
else if(status_code==200)
response->SetStatus(Response::OK);
else if (status_code==404)
response->SetStatus(Response::NOT_FOUND);
else 
response->SetStatus(Response::INTERNAL_SERVER_ERROR);
    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, resp, "\r\n\r\n");

    // Process the response headers.
    std::string currheader;
    while (std::getline(response_stream, currheader) && currheader != "\r")
      {
      	std::string name =currheader.substr(0,currheader.length()-(currheader.length()-currheader.find(':')));
      	std::string value =currheader.substr(currheader.find(':')+2);
      	response->AddHeader(name,value);
      }

       // Read until EOF, writing data to output as we go.
      std::string responsecontent="";
    boost::system::error_code error;
    while (boost::asio::read(socket, resp,
          boost::asio::transfer_all(), error))
      {
      	      
      }
      const char* header=boost::asio::buffer_cast<const char*>(resp.data());                  
   			responsecontent += (std::string)header;
    response->SetBody(responsecontent);
    if (error != boost::asio::error::eof)
      throw boost::system::system_error(error);

return OK;

}