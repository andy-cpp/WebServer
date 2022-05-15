#include "parser.hpp"
#include <stdexcept>

std::vector<HTTP::header_t> HTTPParser::ParseHeaders(std::string const& src)
{
  std::string m_src = src;

  auto parsing_error = std::runtime_error("Error parsing headers");

  size_t header_end = m_src.find("\r\n\r\n");
  if(header_end == std::string::npos)
    throw parsing_error;

  auto lines = split(m_src.substr(0, header_end), "\r\n");

  std::vector<HTTP::header_t> headers;
  for(size_t index = 1; index < lines.size(); ++index)
  {
    std::string line = lines[index];

    int key_pos = line.find(":");
    if(key_pos == std::string::npos)
      throw parsing_error;

    std::string key = line.substr(0, key_pos);
    if(key.empty())
      throw parsing_error;

    std::string value = line.substr(key_pos + 1);
    if(value.empty())
      throw parsing_error;
    if(value[0] == ' ')
      value = value.substr(1);

    headers.push_back({key, value});
  }

  return headers;
}


std::string HTTPParser::GetMessage(std::string const& src)
{
  std::string m_src = src;

  auto parsing_error = std::runtime_error("Error parsing message");
  size_t header_end = m_src.find("\r\n\r\n");
  if(header_end == std::string::npos)
    throw parsing_error;

  return m_src.substr(header_end + 4);
}
