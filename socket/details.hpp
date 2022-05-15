#ifndef DETAILS_HPP
#define DETAILS_HPP

#include <arpa/inet.h>
#include <sys/socket.h>

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <iterator>

class Details
{
public:
  /* version constants */
  static constexpr int IPV4 = 0;
  static constexpr int IPV6 = 1;

  Details()
  {

  }

  Details(const Details& other)
  {
    this->v4_addr = other.v4_addr;
    this->v6_addr = other.v6_addr;
    this->m_port = other.m_port;
    this->m_ip = other.m_ip;
    this->m_version = other.m_version;
  }

  Details& operator=(Details const& other)
  {
    this->v4_addr = other.v4_addr;
    this->v6_addr = other.v6_addr;
    this->m_port = other.m_port;
    this->m_ip = other.m_ip;
    this->m_version = other.m_version;

    return *this;
  }

  Details(unsigned long uIP, int port)
  {
    char str[INET_ADDRSTRLEN] = {0};
    m_version = IPV4;
    m_port = htons(port);

    v4_addr.sin_port = m_port;
    v4_addr.sin_family = AF_INET;
    if(inet_ntop(AF_INET, &(v4_addr.sin_addr), (char*)&str, INET_ADDRSTRLEN) != (void*)&str)
      throw std::runtime_error("[Details::Details]: error converting ip address");
    m_ip = std::string(str);
  }

  Details(std::string const& ip, int port, int version)
  {
    m_ip = ip;
    m_port = htons(port);
    m_version = version;

    if(version == IPV4)
    {
      if(!inet_pton(AF_INET, ip.c_str(), &v4_addr.sin_addr.s_addr))
        throw std::runtime_error("[Details::Details]: error parsing ip address");
      v4_addr.sin_port = m_port;
      v4_addr.sin_family = AF_INET;

    }
    else if(version == IPV6)
    {
      if(!inet_pton(AF_INET6, ip.c_str(), &v6_addr.sin6_addr))
        throw std::runtime_error("[Details::Details]: error parsing ip address");
      v6_addr.sin6_port = m_port;
      v6_addr.sin6_family = AF_INET6;
    }
    else
      throw std::runtime_error("[Details::Details]: invalid ip version");
  }

  std::string GetIP() const
  {
    if(m_version == IPV4)
    {
      char buf[INET_ADDRSTRLEN] = {0};
      if(!inet_ntop(AF_INET, &v4_addr.sin_addr, buf, INET_ADDRSTRLEN))
        throw std::runtime_error("[Details::GetIP]: invalid ip address");
      return std::string(buf);
    }
    else if(m_version == IPV6)
    {
      char buf[INET_ADDRSTRLEN] = {0};
      if(!inet_ntop(AF_INET6, &v6_addr.sin6_addr,buf, INET_ADDRSTRLEN))
        throw std::runtime_error("[Details::GetIP]: invalid ip address");
      return std::string(buf);
    }
    else
      throw std::runtime_error("[Details::GetIP]: invalid ip version");

  }
private:

public:

  sockaddr_in v4_addr;
  sockaddr_in6 v6_addr;
  int m_port;
  int m_version;

private:
  std::string m_ip;

};

#endif
