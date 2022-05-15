#include "responses.hpp"

HTTP::http_t Response_200 = Response::Create({"HTTP/1.1", "200", "OK"}, HEADERS_standard, "");

HTTP::http_t Response_400 = Response::Create({"HTTP/1.1", "400", "Bad Request"}, HEADERS_standard, string_format(HTML_FORMAT_TEMPLATE, "Bad request"));

HTTP::http_t Response_403 = Response::Create({"HTTP/1.1", "403", "Forbidden"}, HEADERS_standard, string_format(HTML_FORMAT_TEMPLATE, "Forbidden"));

HTTP::http_t Response_404 = Response::Create({"HTTP/1.1", "404", "Not Found"}, HEADERS_standard, string_format(HTML_FORMAT_TEMPLATE, "Site not found"));


HTTP::http_t Response_500 = Response::Create({"HTTP/1.1", "500", "Internal server error"}, HEADERS_standard, string_format(HTML_FORMAT_TEMPLATE, "Internal server error"));

std::vector<HTTP::header_t> GetHeaderBasedOnFileExtension(std::string const& filename)
{
  std::string extension;

  int ext_pos = filename.find_last_of(".");
  if(ext_pos == std::string::npos)
    extension = "";
  else
    extension = filename.substr(ext_pos + 1);

  if(extension == "js")
  {
      return {{"Content-Type", "text/javascript; charset=UTF-8"}};
  }

  if(extension.empty())
    return HEADERS_standard;

  if(extension == "img" || extension == "ico" || extension == "jpg" || extension == "jpeg")
    return HEADERS_image;

  return HEADERS_standard;
}


std::vector<HTTP::header_t> CombineHeaders(std::vector<HTTP::header_t> const& first, std::vector<HTTP::header_t> const& second)
{
    std::vector<HTTP::header_t> result = first;

    for(auto& sheader : second)
    {
        bool bFound = false;

        for(auto& header : result)
        {
            if(sheader.key == header.key)
                bFound = true;

        }
        if(!bFound)
          result.push_back(sheader);
    }

    return result;
}


#include <filesystem/fs.hpp>
using namespace FS;
std::string CreateDirectoryIndex(std::string const& directoryname, std::string const& path)
{
    auto names = FS::GetNamesInDirectory(directoryname);

    std::string output = "<html><head></head><body><ul>";
    for(auto& name : names)
    {

      std::string encoded_name = HTMLEncode(name);
      char const* cptr = encoded_name.c_str();
      output += string_format("<li><a href=\"%s\">%s</a></li>", cptr,cptr);
    }
    output += "</ul></body></html>";
    return output;
}