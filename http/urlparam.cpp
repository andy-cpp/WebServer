#include "urlparam.hpp"
#include "urlencode.hpp"
#include <utils.hpp>

using namespace http;

std::string GetQueryString(std::string const& uri)
{
    if(uri.find("?") == std::string::npos)
        return "";

    return uri.substr(uri.find("?") + 1);
}
#include <iostream>

std::pair<std::string, std::string> GetParam(std::string const& param)
{
    std::string key;
    std::string value;
    if(param.find("=") == std::string::npos)
        key = param;
    else
    {
        key = param.substr(0, param.find("="));
        value = param.substr(param.find("=") + 1);
    }
    
    return {key, value};
}

urlparams_t http::DecodeURLParams(std::string const& encoded_uri)
{
    urlparams_t params;
    std::string query_string = GetQueryString(encoded_uri);

    auto parameters = split(query_string, "&");

    for(auto& paramstring : parameters)
    {
        auto param = GetParam(paramstring);
        params[param.first] = http::DecodeURIComponent(param.second);
    }

    return params;
}
