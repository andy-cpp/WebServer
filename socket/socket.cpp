#include "socket.hpp"

#include "../utils.hpp"
#include "../debug.hpp"

Socket::Socket()
{

}


Socket::Socket(Details const& details, int type)
{
  m_protocol = (type == UDP) ? SOCK_DGRAM : SOCK_STREAM;
  int family = (details.m_version == details.IPV4) ? AF_INET : AF_INET6;

  m_sockfd = socket(family, m_protocol, 0);

  m_details = details;
}

Socket::~Socket()
{

}

int Socket::GetSocket() const
{
  return m_sockfd;
}

bool Socket::wrapsocket(const SSL_METHOD*(*pmethodfunc)())
{
  const SSL_METHOD* method = pmethodfunc();


  m_ssl_ctx = SSL_CTX_new(method);
  if(m_ssl_ctx == 0)
    return false;

  m_ssl = SSL_new(m_ssl_ctx);
  SSL_set_fd(m_ssl, m_sockfd);
  m_bSSL = true;
  return m_ssl != 0;
}


bool Socket::Connect()
{
  bool status;
  if(m_details.m_version == Details::IPV4)
    status = connect(m_sockfd, (struct sockaddr*)&m_details.v4_addr, sizeof(m_details.v4_addr)) == 0;
  else if(m_details.m_version == Details::IPV6)
    status = connect(m_sockfd, (struct sockaddr*)&m_details.v6_addr, sizeof(m_details.v6_addr)) == 0;
  else
    throw std::runtime_error("[Socket::Bind]: ip version error");

  if(m_bSSL && status)
  {
    return _SSL::Connect(m_ssl);
  }
  return status;
}

bool Socket::Bind()
{
  if(m_details.m_version == Details::IPV4)
    return bind(m_sockfd, (struct sockaddr*)&m_details.v4_addr, sizeof(m_details.v4_addr)) == 0;
  else if(m_details.m_version == Details::IPV6)
    return bind(m_sockfd, (struct sockaddr*)&m_details.v6_addr, sizeof(m_details.v6_addr)) == 0;
  else
      throw std::runtime_error("[Socket::Bind]: ip version error");
}

bool Socket::Listen(int backlog)
{
  return listen(m_sockfd, backlog) == 0;
}

size_t Socket::Send(std::string const& data, size_t size /* = 0 */)
{
  if(m_bSSL)
    return _SSL::Write(m_ssl, data);

  if(size == 0)
    size = data.size();

  size_t bytes_sent = 0;
  /* send in chunks of 4096 bytes */
  float fTimes = ((float)size) / 4096;
  unsigned int iTimes = size / 4096;
  if(iTimes < 1 || fTimes > iTimes)
    ++iTimes;

  for(size_t index = 0; index < iTimes; ++index)
  {
    int size = data.size() - (index * 4096);
    if(size > 4096)
      size = 4096;
    char const* buffer = data.c_str() + (index * 4096);

    size_t status = send(m_sockfd, buffer, size, 0);
    if(status < 0)
      throw std::runtime_error("[Socket::Send]: error writing data to socket");
    bytes_sent += status;
    if(status < 4096)
      break;
  }

  return bytes_sent;
}

size_t Socket::Send(std::ostringstream& data, size_t size /* = 0 */)
{
  if(m_bSSL)
    return _SSL::Write(m_ssl, data.str());
  if(size == 0)
  size = strsize(data);

  size_t bytes_sent = 0;
  /* send in chunks of 4096 bytes */
  double fTimes = ((float)size) / 4096;
  unsigned int iTimes = size / 4096;
  if(iTimes < 1 || fTimes > iTimes)
    ++iTimes;

  for(size_t index = 0; index < iTimes; ++index)
  {
    int size = strsize(data) - (index * 4096);
    if(size > 4096)
      size = 4096;
    char const* buffer = data.str().c_str() + (index * 4096);


    size_t status = send(m_sockfd, buffer, size, 0);
    if(status < 0)
      throw std::runtime_error("[Socket::Send]: error writing data to socket");
    bytes_sent += status;
    if(status < 4096)
      break;
  }

  return bytes_sent;
}


size_t Socket::Receive(std::string& buffer)
{
  if(m_bSSL)
    return _SSL::Read(m_ssl, buffer);

  size_t bytes_received = 0;

  char* buf = (char*)malloc(4096);
  if(buf == 0)
    throw std::runtime_error("[Socket::Receive]: nullptr exception");

  while(true)
  {
    memset(buf, 0, 4096);
    int status = recv(m_sockfd, buf, 4096, 0);
    if(status < 0)
      throw std::runtime_error("[Socket::Receive]: error reading data from socket");
    bytes_received += status;
    buffer.append(buf, status);
    if(status < 4096)
      break;
  }

  free(buf);
  return bytes_received;
}

size_t Socket::Receive(std::ostringstream& buffer)
{
  size_t bytes_received = 0;

  char* buf = (char*)malloc(4096);
  if(buf == 0)
    throw std::runtime_error("[Socket::Receive]: nullptr exception");

  while(true)
  {
    memset(buf, 0, 4096);
    int status = recv(m_sockfd, buf, 4096, 0);
    if(status < 0)
      throw std::runtime_error("[Socket::Receive]: error reading data from socket");
    bytes_received += status;
    buffer << buf;
    if(status < 4096)
      break;
  }

  free(buf);
  return bytes_received;
}

bool Socket::Accept(Socket& buf)
{
  if(m_details.m_version == Details::IPV4)
  {
    socklen_t len = sizeof(buf.m_details.v4_addr);
    int status = accept(m_sockfd, (struct sockaddr*)&buf.m_details.v4_addr, &len);
    if(status < 0)
      return false;
    buf.m_details.m_version = Details::IPV4;
    buf.m_sockfd = status;

  }
  else if(m_details.m_version == Details::IPV6)
  {
    socklen_t len = sizeof(buf.m_details.v6_addr);
    int status = accept(m_sockfd, (struct sockaddr*)&buf.m_details.v6_addr, &len);
    if(status < 0)
      return false;
    buf.m_details.m_version = Details::IPV6;
    buf.m_sockfd = status;

  }
  else
    throw std::runtime_error("[Socket::Accept]: ip version error");

  return true;
}

size_t Socket::SendTo(Details const& details, std::string const& data)
{
  if(m_bSSL)
    return _SSL::Write(m_ssl, data);

  size_t bytes_sent = 0;
  /* send in chunks of 4096 bytes */
  double fTimes = data.size() / 4096;
  unsigned int iTimes = data.size() / 4096;
  if(iTimes < 1 || fTimes > iTimes)
    ++iTimes;

  sockaddr* psockaddr;
  socklen_t len;

  if(details.m_version == Details::IPV4){
    psockaddr = (sockaddr*)&details.v4_addr;
    len = sizeof(details.v4_addr);
  }
  else if(details.m_version == Details::IPV6){
    psockaddr = (sockaddr*)&details.v6_addr;
    len = sizeof(details.v6_addr);
  }
  else
    throw std::runtime_error("[Socket::SendTo]: invalid ip version");


  for(size_t index = 0; index < iTimes; ++index)
  {
    int size = data.size() - (index * 4096);
    if(size > 4096)
      size = 4096;
    char const* buffer = data.c_str() + (index * 4096);

    int status = sendto(m_sockfd, buffer, size, 0, (const sockaddr*)psockaddr, len);
    if(status < 0)
      break;
    bytes_sent += status;
    if(bytes_sent < 4096)
      break;

  }

  return bytes_sent;
}

size_t Socket::ReceiveFrom(Details const& details, std::string& buffer)
{
  size_t bytes_received = 0;

  char* buf = (char*)malloc(4096);
  if(buf == 0)
    throw std::runtime_error("[Socket::ReceiveFrom]: nullptr exception");

  sockaddr* psockaddr;
  socklen_t len;

  if(details.m_version == Details::IPV4){
    psockaddr = (sockaddr*)&details.v4_addr;
    len = sizeof(details.v4_addr);
  }
  else if(details.m_version == Details::IPV6){
    psockaddr = (sockaddr*)&details.v6_addr;
    len = sizeof(details.v6_addr);
  }
  else
    throw std::runtime_error("[Socket::ReceiveFrom]: invalid ip version");


  while(true)
  {
    memset(buf, 0, 4096);
    int status = recvfrom(m_sockfd, buf, 4096, 0, psockaddr, &len);
    if(status < 0)
      break;
    bytes_received += status;
    buffer.append(buf);
    if(status < 4096)
      break;
  }

  return bytes_received;
}

bool Socket::Close()
{
  if(m_bSSL)
    _SSL::Close(m_ssl);
  else
    return close(m_sockfd) == 0;

  return true;
}
