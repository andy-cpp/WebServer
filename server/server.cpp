#include "server.hpp"

#include <thread>
#include "handler.hpp"

Server::Server(Details const& details, std::string const& config_filename)
{
  m_socket = Socket(details, Socket::TCP);
  m_socket.Setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
  m_socket.Setsockopt(SOL_SOCKET, SO_REUSEPORT, 1);

  m_config_filename = config_filename;
  init();


}

void Server::init()
{
  m_config = Config(m_config_filename);
  if(!VerifyConfig()){
    exit(1);
  }

  logger = Logger(m_config.Get("logfile").ToString());
}

Server::Server(std::string const& config_filename)
{
  m_config_filename = config_filename;
  init();
}


Server::~Server()
{

}

void Server::InitializeSSL()
{
  SSL_CTX* ctx = SSL_CTX_new(SSLv23_method());

  if(!m_config.exists("ssl_certfile") || !m_config.exists("ssl_keyfile"))
  {
      ERROR_FATAL("Provide ssl_certfile and ssl_keyfile in your config file!");
  }


  std::string certfile = m_config["ssl_certfile"].ToString();
  std::string keyfile = m_config["ssl_keyfile"].ToString();

  if(certfile.empty() || keyfile.empty())
  {
    ERROR_FATAL("Provide ssl_certfile and ssl_keyfile in your config file!");
  }
  _SSL::LoadCertificates(ctx, certfile.c_str(), keyfile.c_str());

  std::string bundlefile = m_config["ssl_bundle"].ToString();

  if(m_config.exists("ssl_bundle") && bundlefile.empty()){
    ERROR_FATAL("Please provide a value for ssl_bundle");
  }
  else
    SSL_CTX_load_verify_locations(ctx, bundlefile.c_str(), 0);


  m_ssl_ctx = ctx;
  m_socket.m_ssl_ctx = ctx;
}

void Server::InitDetails()
{
  try {
    m_details = Details(m_config["ip"].ToString(), m_config["port"].ToNumber(), Details::IPV4);

    m_socket = Socket(m_details, Socket::TCP);
  }
  catch(std::exception& exception)
  {

    error(exception.what());
  }
}

void Server::InitSocket(Details* pdetails)
{
  Details details = (pdetails == 0) ? m_details : *pdetails;

  m_socket = Socket(details, Socket::TCP);
  m_socket.Setsockopt(SOL_SOCKET, SO_REUSEADDR, 1);
  m_socket.Setsockopt(SOL_SOCKET, SO_REUSEPORT, 1);

}

bool Server::IsSSL()
{
  m_bSSL = m_config["ssl"].ToBool();
  return m_bSSL;
}

void Server::Start()
{
  printf("[LOG]: starting server on port %d\n", ntohs(m_socket.m_details.m_port));

  if(this->IsSSL())
  {
    this->InitializeSSL();
  }

  m_running = true;
  try
  {
    if(!m_socket.Bind() || !m_socket.Listen())
      throw std::runtime_error("[Server::Start]: error starting server");

    while(m_running)
    {
      Socket* client = new Socket;
      if(!m_socket.Accept(*client))
        throw std::runtime_error("[Server::Start]: error accepting client");

      /* set up ssl if necessary */
      if(m_bSSL)
      {
        client->m_ssl_ctx = m_ssl_ctx;
        client->m_ssl = SSL_new(m_ssl_ctx);
        SSL_set_fd(client->m_ssl, client->m_sockfd);
        client->m_bSSL = true;

        if(SSL_accept(client->m_ssl) == -1)
        {
          client->Close();
          ERR_print_errors_fp(stdout);
          continue;
        };
      }

      /* pass socket to our handler */
      std::thread(Handler::handler_thread, this, client, &m_config).detach();
    }

  }
  catch(std::exception& error)
  {
    printf("--- error ---\n%s\n", error.what());
    printf("%s\n", strerror(errno));
  }
}

bool StringIsTrue(std::string const& str)
{
  return str == "true" || str == "True" || str == "TRUE";
}

bool Server::VerifyConfig()
{
  bool status = true;

  for(auto const& key : MandatoryConfigKeys)
  {
    if(!m_config.exists(key))
    {
      status = false;

      printf("[ERROR]: %s key is missing in %s\n", key.c_str(), m_config.m_filename.c_str());
      /* print error */
    }
  }
  if(!status){
    ERROR("Server was not started!");
    return false;
  }

  if(!m_config.exists("logfile"))
    m_config["logfile"] = "./log.txt";

  if(!m_config.exists("webroot")){
    printf("[ERROR]: webroot can not be empty\n");
    return false;
  }
  m_config["webroot"] = GetRealPath(m_config["webroot"].ToString());

  if(!FS::IsDirectory(m_config["webroot"].ToString())){
    printf("[ERROR]: %s directory not found\n", m_config["webroot"].ToString().c_str());
    return false;
  }
  if(m_config["webroot"].ToString().back() != '/')
    m_config["webroot"].ToString().push_back('/');

  /* Initialize proxy settings? */
  if(m_config.exists("proxy"))
  {
    auto parts = split(m_config.Get("proxy").ToString(), ":");
    ASSERT(parts.size() == 2);

    m_Proxyhost = parts[0];
    m_Proxyport = std::stoi(parts[1]);
    m_bUsingProxy = true;
  }

  return status;
}


std::vector<std::string> Server::GetConfigFiles()
{
  if(!FS::IsFile("configs.json"))
    throw std::runtime_error("[Server::GetConfigFiles]: 'configs' file not found");

  std::string data;
  if(FS::read("configs.json", data) < 1)
    throw std::runtime_error("[Server::GetConfigFiles]: no config files found");

  auto config_file = JSON::Parse(data);
  std::vector<std::string> Configs = {};

  for(Object const& entry : config_file.ToArray())
  {
    Configs.push_back(entry.ToString());
  }

  // Check for duplicates
  std::sort(Configs.begin(), Configs.end());
  auto it = std::unique(Configs.begin(), Configs.end());
  bool bDuplicates = it != Configs.end();

  if(bDuplicates){
    ERROR_FATAL("There are duplicate configs in ./configs");
  }


  return Configs;
}
