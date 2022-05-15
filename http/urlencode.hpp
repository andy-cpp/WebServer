#pragma once
#include <string>

namespace http
{
    std::string DecodeURIComponent(std::string const& encoded);

    std::string EncodeURIComponent(std::string const& decoded);
}