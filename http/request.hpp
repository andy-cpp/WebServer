#pragma once

#include "http.hpp"
#include "parser.hpp"

namespace Request {
  inline HTTP::http_t Create(RequestParser::request_line req_line, std::vector<HTTP::header_t> const& headers, std::string const& message)
  {
    HTTP::http_t http;

    http.http_version = req_line.http_version;
    http.http_request_method = req_line.http_method;
    http.http_path = req_line.http_path;

    http.headers = headers;
    http.message = message;

    return http;
  }

  inline std::string ToString(HTTP::http_t const& http)
  {
    std::string line = http.http_request_method + " " + http.http_path + " " + http.http_version + "\r\n";

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