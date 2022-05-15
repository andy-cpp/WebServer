#ifndef SERVER_HPP
#define SERVER_HPP

#include "../socket/socket.hpp"
#include "../config/config.hpp"
#include "../debug.hpp"
#include "handler.hpp"
#include "logging.hpp"

const std::vector<std::string> MandatoryConfigKeys = {
  "port",
  "ip",
  "ssl",
  "webroot",
  "default_site"
};


class Server
{
public:
  Server(Details const& details, std::string const& config_filename);

  Server(std::string const& config_filename);

  ~Server();

  void InitializeSSL();

  void InitDetails();

  void InitSocket(Details* pdetails = 0);

  bool IsSSL();

  void Start();

  bool VerifyConfig();

  

  /* gets config files */
  static std::vector<std::string> GetConfigFiles();

private:
  void init(); // common inits

private:
  Details m_details;

  

  Socket m_socket;
  std::string m_config_filename;
  bool m_running = false;
  bool m_bSSL = false;
  SSL_CTX* m_ssl_ctx = 0;

  // Proxy variables
public:
  Logger logger;
  Config m_config;
  std::string m_Proxyhost;
  unsigned short m_Proxyport;
  bool m_bUsingProxy = false;
};



#endif
