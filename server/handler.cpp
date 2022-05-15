#include "handler.hpp"

#include "../utils.hpp"
#include "server.hpp"


void Handler::handler_thread(Server* server, Socket* socket, Config* config)
{
  try {
  HTTP::http_t request = Handler::GetRequest(socket);

  Handler::handle_request(server, socket, &request, config);

  socket->Close();
  }
  catch(std::exception& error){
    printf("[CLIENT ERROR]: %s\n", error.what());
  }

}

bool LFIFilter(Server* server, std::string const& filename)
{
  printf("[LFIFilter]: filename: %s\n", filename.c_str());

  if(filename.empty())
    return true;

  if(contains(filename, "../") || contains(filename, "./"))
    return false;

  char cbuf[PATH_MAX + 1] = {0};
  realpath((server->m_config.Get("webroot").ToString() + "/" + filename).c_str(), cbuf);

  return startswith(cbuf, server->m_config.Get("webroot").ToString());

  return true;
}


void SendResponse(HTTP::http_t const& res, Socket* client)
{
  client->Send(Response::ToString(res));
}

#include "php.hpp"

void Handler::handle_request(Server* server, Socket* socket, HTTP::http_t* request, Config* config)
{
  std::string filename = request->GetFile();
  if(!LFIFilter(server, filename))
    return (void)socket->Send(Response::ToString(Response_403));

  filename = (filename.empty()) ? config->Get("default_site").ToString() : filename;

  std::string realpath = config->Get("webroot").ToString() + filename;

  if(endswith(filename, ".php"))
  {
    return (void)PHPHandler(server, socket, request);
  }

  if(server->m_bUsingProxy)
    return (void)proxyhandler(server,socket,request,config);

  if(FS::IsDirectory(realpath)){
    auto resp = Response_200;
    resp.message = CreateDirectoryIndex(server->m_config.Get("webroot").ToString() + filename,filename);
    return (void)socket->Send(Response::ToString(resp));
  }
  if(!FS::IsFile(realpath))
    return (void)socket->Send(Response::ToString(Response_404));

  if(!SendFile(socket, realpath, "200", "OK")){
    throw std::runtime_error(std::string("error sending file ") + filename);
  }


}

void Handler::proxyhandler(Server* server, Socket* client, HTTP::http_t* request, Config* config)
{    
    Socket proxy = Socket(Details(server->m_Proxyhost, server->m_Proxyport, Details::IPV4), Socket::TCP);    
    
    if(!proxy.Connect())
      return (void)SendResponse(Response_500, client);

    if(!proxy.Send(Request::ToString(*request)))
      return (void)SendResponse(Response_500, client);

    std::string buffer;
    int bytes = proxy.Receive(buffer);
    if(bytes <= 0)
      return (void)SendResponse(Response_500, client);
    
    client->Send(buffer);

}
#include <http/urlencode.hpp>
HTTP::http_t Handler::GetRequest(Socket* socket)
{
  std::string data;
  if(socket->Receive(data) < 1)
    throw std::runtime_error("[Handler::GetRequest]: error reading data");

  RequestParser parser(data);
  

  auto request = parser.parse();
  request.http_path = http::DecodeURIComponent(request.http_path);
  return request;
}

void Handler::SendError(Socket* socket, std::string const& http_code, std::string const& http_response_status, std::string const& http_message)
{
  auto resp = Response::Create({"HTTP/1.1", http_code, http_response_status}, HEADERS_standard, http_message);
  socket->Send(Response::ToString(resp));
}


bool Handler::SendFile(Socket* psocket, std::string const& filename, std::string http_code, std::string http_status)
{
  auto headers = GetHeaderBasedOnFileExtension(filename);

  if(!FS::IsFile(filename))
    return false;

  std::string buffer;
  int bytes = FS::read(filename, buffer);
  if(bytes < 0)
    return false;

  auto response = Response::Create({"HTTP/1.1", "200","OK"}, headers, buffer);
  psocket->Send(Response::ToString(response));

  return true;
}
