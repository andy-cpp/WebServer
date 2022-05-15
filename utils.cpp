#include "utils.hpp"



std::string HTMLEncode(std::string const& data)
{
    std::string encoded;

    for(auto& c : data)
    {
        if(!isalpha(c))
        {
            encoded.append("&#" + std::to_string(c) + ";");
        }
        else encoded.push_back(c);
    }
    return encoded;
}