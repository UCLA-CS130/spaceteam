#ifndef SERVER_STATUS_H
#define SERVER_STATUS_H

#include <string>
#include <map>
#include <vector>

#include "response.h"

class ServerStatus {
public:

  ServerStatus();

	void UpdateStatus(std::string uri_prefix, Response::ResponseCode status);

	std::string ToHtml();

	std::string ToString();

  void AddHandlerInfo(std::map<std::string, std::vector<std::string>> info);

private:
	// Maps URI prefix to status code and number of requests
	std::map<std::string, std::map<int,unsigned>> server_status_;

  std::map<std::string, std::vector<std::string>> handler_name_to_prefixes_;

  unsigned num_requests_;

  unsigned ResponseCodeToString(Response::ResponseCode code);
};

#endif