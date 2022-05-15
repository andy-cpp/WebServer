#ifndef PARSER_HPP
#define PARSER_HPP


#include "http.hpp"
#include "../utils.hpp"

#include <stdexcept>


namespace HTTPParser
{
  std::vector<HTTP::header_t> ParseHeaders(std::string const& src);

  std::string GetMessage(std::string const& src);

};

class RequestParser
{
public:
  typedef struct {
    std::string http_method, http_path, http_version;
  }request_line;

  RequestParser(std::string const& src)
  {
    m_src = src;
  }

  HTTP::http_t parse()
  {
    auto requestline = this->ParseFirstLine();
    auto headers = HTTPParser::ParseHeaders(m_src);
    auto message = HTTPParser::GetMessage(m_src);

    HTTP::http_t http;
    http.message = message;
    http.headers = headers;
    http.http_request_method = requestline.http_method;
    http.http_path = requestline.http_path;
    http.http_version = requestline.http_version;

    if(http.http_path.find("?") != std::string::npos){
      http.query = http.http_path.substr(http.http_path.find("?") + 1);
      http.http_path = http.http_path.substr(0, http.http_path.find("?"));
    }

    return http;
  }

  request_line ParseFirstLine()
  {
    auto parsing_error = std::runtime_error("Error parsing first line");

    auto lines = split(m_src, "\r\n");
    if(lines.size() < 1)
      throw parsing_error;

    std::string line = lines[0];
    auto words = split(line, " ");
    if(words.size() != 3)
      throw parsing_error;

    for(auto& c : words[0])
    {
      if((c >= 'a' && c <= 'z') || (c >= 'A' && c <='Z'))
        continue;
      else throw parsing_error;
    }

    request_line req_line = {words[0], words[1], words[2]};  

    return req_line;
  }

  std::string GetFile()
  {
    std::string path = ParseFirstLine().http_path;
    
    if(path.find("?") != std::string::npos)
      return path.substr(1, path.find("?"));

    return path.substr(1);

    
  }
private:
  std::string m_src;
};




class ResponseParser
{
public:
  typedef struct {
    std::string http_version, http_code, http_response_status;
  }response_line;

  ResponseParser(std::string const& src)
  {
    m_src = src;
  }

  HTTP::http_t parse()
  {
    auto requestline = this->ParseFirstLine();
    auto headers = HTTPParser::ParseHeaders(m_src);
    auto message = HTTPParser::GetMessage(m_src);

    HTTP::http_t http;
    http.message = message;
    http.headers = headers;
    http.http_code = requestline.http_code;
    http.http_response_status = requestline.http_response_status;
    http.http_version = requestline.http_version;

    return http;
  }

  response_line ParseFirstLine()
  {
    auto parsing_error = std::runtime_error("Error parsing first line");

    auto lines = split(m_src, "\r\n");
    if(lines.size() < 1)
      throw parsing_error;

    std::string line = lines[0];
    auto words = split(line, " ");
    if(words.size() < 3)
      throw parsing_error;

    std::string response_status;
    int first = 0;
    for(size_t index = 2; index < words.size(); ++index)
      response_status += (first++ == 0 ? "" : " ") + words[index];

    response_line req_line = {words[0], words[1], response_status};
    return req_line;
  }
private:
  std::string m_src;
};



#endif
