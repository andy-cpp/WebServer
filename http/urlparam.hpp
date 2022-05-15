#pragma once

#include <map>

namespace http
{
    typedef std::map<std::string, std::string> urlparams_t;

    urlparams_t DecodeURLParams(std::string const& encoded_uri);
}