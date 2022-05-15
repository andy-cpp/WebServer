#pragma once

#include "../utils.hpp"

namespace HTTP
{


  typedef struct {
    std::string key, value;
  }header_t;

  typedef struct {
    std::string http_code, http_version, http_path, http_request_method, http_response_status;
    std::string message;
    std::string query = ""; // ? query string :)


    std::vector<header_t> headers;

    inline std::string GetFile() const
    {
      if(http_path.find("?") != std::string::npos)
        return http_path.substr(1, http_path.find("?"));

      return http_path.substr(1);
    }
  }http_t;
}
