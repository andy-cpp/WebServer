#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "http.hpp"
#include "parser.hpp"

namespace Response {
  inline HTTP::http_t Create(ResponseParser::response_line resp_line, std::vector<HTTP::header_t> const& headers, std::string const& message)
  {
    HTTP::http_t http;

    http.http_version = resp_line.http_version;
    http.http_response_status = resp_line.http_response_status;
    http.http_code = resp_line.http_code;

    http.headers = headers;
    http.message = message;

    return http;
  }

  inline std::string ToString(HTTP::http_t const& http)
  {
    std::string line = http.http_version + " " + http.http_code + " " + http.http_response_status + "\r\n";

    for(auto const& header : http.headers)
    {
      line += header.key + ": " + header.value + "\r\n";
    }
    line += "\r\n";
    line += http.message;
    line += "\r\n\r\n";

    return line;
  }

}



#endif
