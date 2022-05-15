#ifndef HANDLER_HPP
#define HANDLER_HPP

#include "../socket/socket.hpp"
#include "../debug.hpp"
#include "../http/http.hpp"
#include "../http/parser.hpp"
#include "../http/response.hpp"
#include "../http/request.hpp"
#include "../filesystem/fs.hpp"
#include "../config/config.hpp"

#include <sstream>
#include <iostream>
#include <string>

#include "responses.hpp"

struct Server;


namespace Handler
{
  typedef bool(*handler_t)(Server* server, Socket* client, HTTP::http_t* request);

  void handler_thread(Server* server, Socket* socket, Config* config);

  void handle_request(Server* server, Socket* socket, HTTP::http_t* request, Config* config);

  void proxyhandler(Server* server, Socket* client, HTTP::http_t* request, Config* config);

  HTTP::http_t GetRequest(Socket* socket);

  void SendError(Socket* socket, std::string const& http_code, std::string const& http_response_status, std::string const& http_message);

  bool SendFile(Socket* socket, std::string const& filename, std::string http_code, std::string http_status);

}


#endif
