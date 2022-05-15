#include "ssl.hpp"

#include "../debug.hpp"

#include <iostream>
#include <stdexcept>

void _SSL::Init()
{
  SSL_library_init();
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();

}

bool _SSL::Accept(SSL* ssl)
{
  return (SSL_accept(ssl) == 1);
}

bool _SSL::Connect(SSL* ssl)
{
  return SSL_connect(ssl) != -1;
}


void _SSL::Close(SSL* ssl)
{
  int fd = SSL_get_fd(ssl);
  SSL_free(ssl);
  close(fd);
}

long long _SSL::Read(SSL* ssl, std::string& buf)
{
  char* buffer = (char*)malloc(4096);
  if(buffer == 0)
    throw std::runtime_error("[SSL::Read]: nullptr exception");

  long long bytes_read = 0;
  while(1)
  {
    int bytes = SSL_read(ssl, buffer, 4096);
    if(bytes < 0)
      break;
    bytes_read += bytes;
    buf.append(buffer, bytes);
    if(bytes < 4096)
      break;
  }

  free(buffer);
  return bytes_read;
}

long long _SSL::Write(SSL* ssl, std::string const& data, size_t size /* = 0 */)
{
  if(size == 0)
    size = data.size();

  double fTimes = ((double)size / 4096);
  int iTimes = size / 4096;

  if(iTimes < 1 || fTimes > iTimes)
    ++iTimes;

  long long bytes_written = 0;
  for(size_t index = 0; index < size; ++index)
  {
    int len = size - (index * 4096);
    if(len > 4096)
      len = 4096;
    char const* buffer = data.c_str() + (index * 4096);

    int bytes = SSL_write(ssl, buffer, len);
    if(bytes < 0)
      break;

    bytes_written += bytes;
    if(bytes < 4096)
      break;
  }

  return bytes_written;
}


SSL_CTX* _SSL::InitCTX()
{
  const SSL_METHOD* method;
  SSL_CTX* ctx;

  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();
  method = TLSv1_2_client_method();
  ctx = SSL_CTX_new(method);
  if(ctx == 0)
  {
    ERR_print_errors_fp(stdout);
    exit(1);
  }
  return ctx;
}

SSL_CTX* _SSL::InitServerCTX()
{
  const SSL_METHOD* method;
  SSL_CTX *ctx;

  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();
  method = TLSv1_2_server_method();
  ctx = SSL_CTX_new(method);
  if (ctx == 0)
  {
      ERR_print_errors_fp(stdout);
      exit(1);
  }
  return ctx;
}

void _SSL::LoadCertificates(SSL_CTX* ctx, char const* certfile, char const* keyfile)
{
  /* set the local certificate from CertFile */
  if (SSL_CTX_use_certificate_file(ctx, certfile, SSL_FILETYPE_PEM) <= 0)
  {
      ERR_print_errors_fp(stdout);
      exit(1);
  }
  /* set the private key from KeyFile (may be the same as CertFile) */
  if (SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM) <= 0)
  {
      ERR_print_errors_fp(stdout);
      exit(1);
  }
  /* verify private key */
  if (!SSL_CTX_check_private_key(ctx))
  {
      fprintf(stdout, "Private key does not match the public certificate\n");
      exit(1);
  }
}


void _SSL::ShowCerts(SSL* ssl)
{
  X509* cert;
  char* line;

  cert = SSL_get_peer_certificate(ssl);
  if(cert != 0)
  {
    printf("Server certificates:\n");
    line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
    printf("Subject: %s\n", line);
    free(line);       /* free the malloc'ed string */
    line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
    printf("Issuer: %s\n", line);
    free(line);       /* free the malloc'ed string */
    X509_free(cert);     /* free the malloc'ed certificate copy */
  }
    printf("Info: No client certificates configured.\n");
}
