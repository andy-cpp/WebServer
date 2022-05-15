#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <sstream>
#include "details.hpp"

#include "ssl.hpp"

class Socket
{
public:
  // protocol type constants
  static const int UDP = 0;
  static const int TCP = 1;

  Socket();

  Socket(Details const& details, int type);

  ~Socket();

  bool wrapsocket(const SSL_METHOD*(*pmethodfunc)());


  int GetSocket() const;

  bool Connect();

  bool Bind();

  bool Listen(int backlog = 0);

  size_t Send(std::string const& data, size_t size = 0);

  size_t Send(std::ostringstream& data, size_t size = 0);

  size_t Receive(std::string& buffer);

  size_t Receive(std::ostringstream& buffer);

  bool Accept(Socket& buf);

  size_t SendTo(Details const& details, std::string const& data);

  size_t ReceiveFrom(Details const& details, std::string& buffer);

  bool Close();

  template <typename T>
  bool Setsockopt(int level, int optname, T const& value)
  {
    return setsockopt(m_sockfd, level, optname, &value, sizeof(value)) == 0;
  }

  template <typename T>
  bool Getsockopt(int level, int optname, T& value)
  {
    return getsockopt(m_sockfd, level, optname, &value, sizeof(value)) == 0;
  }

  bool settimeout(unsigned long long ms)
  {
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = ms * 1000;
    if(setsockopt(m_sockfd, SOL_SOCKET, SO_RCVTIMEO, (char const*)&timeout, sizeof(timeout)) != 0)
      return false;

    return true;
  }


public:
  int m_sockfd;
  Details m_details;
  int m_protocol;

public:
  bool m_bSSL = false;
  SSL* m_ssl = 0;
  SSL_CTX* m_ssl_ctx = 0;
};


#endif
