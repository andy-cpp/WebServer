#pragma once

#include <vector>
#include "../http/response.hpp"
#include "../http/request.hpp"
#include <http/http.hpp>


/* Vector of headers depending on file extension */
inline const std::vector<HTTP::header_t> HEADERS_image = {
  {"Content-Type","image/x-icon"},
  {"accept-ranges","bytes"},
  {"vary","Accept-Encoding"}
};

inline const std::vector<HTTP::header_t> HEADERS_standard = {
  {"Content-Type", "text/html"}
};

#define HTML_FORMAT_TEMPLATE "<html><head></head><body>%s</body></html>"

std::vector<HTTP::header_t> GetHeaderBasedOnFileExtension(std::string const& filename);

std::vector<HTTP::header_t> CombineHeaders(std::vector<HTTP::header_t> const& first, std::vector<HTTP::header_t> const& second);

std::string CreateDirectoryIndex(std::string const& directory, std::string const& path);

extern HTTP::http_t Response_200;

extern HTTP::http_t Response_400, Response_403, Response_404;



extern HTTP::http_t Response_500;
