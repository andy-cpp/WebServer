#ifndef SSL_HPP
#define SSL_HPP

#include <sys/socket.h>
#include <string>
#include <unistd.h>

extern "C" {
#include <openssl/ssl.h>
#include <openssl/err.h>
}

namespace _SSL
{
  void Init();

  bool Accept(SSL* ssl);

  bool Connect(SSL* ssl);

  void Close(SSL* ssl);

  long long Read(SSL* ssl, std::string& buffer);

  long long Write(SSL* ssl, std::string const& data, size_t size = 0);

  SSL_CTX* InitCTX();

  SSL_CTX* InitServerCTX();

  void LoadCertificates(SSL_CTX* ctx, char const* certfile, char const* keyfile);

  void ShowCerts(SSL* ssl);
}

#endif /* end of include guard: SSL_HPP */
