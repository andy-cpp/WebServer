#include "urlencode.hpp"
#include <utils.hpp>

std::string EncodeChar(unsigned char c)
{
    return "%" + string_format("%02X", c);
}

unsigned char DecodeChar(std::string const& character)
{
    if(character.empty() || character[0] != '%' || character.size() != 3)
        throw std::runtime_error("[DecodeChar]: error decoding character");

    try {
        return std::stoi(character.substr(1),0, 16);
    }
    catch(...){
        throw std::runtime_error("[DecodeChar]: error decoding character");

    }
}


std::string http::DecodeURIComponent(std::string const& encoded)
{
    std::string decoded;

    unsigned index = 0;
    while(index < encoded.length())
    {
        char character = encoded[index];

        if(character == '%'){
            decoded += DecodeChar(encoded.substr(index, 3));
            index += 2;
        }
        else
            decoded += character;

        index += 1; 
    }
    return decoded;

}

static std::string GetPath(std::string uri)
{
    // skip http scheme 
    if(startswith(uri, "http://") || startswith(uri, "https://"))
    {
        uri = uri.substr(uri.find("://") + 3);
    }
    if(uri.find("/") == std::string::npos)
        return "";

    return uri.substr(uri.find("/") + 1);
}
#include <iostream>

std::string http::EncodeURIComponent(std::string const& uri_decoded)
{
    std::string path = GetPath(uri_decoded);
    std::cout << path << "\n";
    std::string encoded = uri_decoded.substr(0, uri_decoded.length() - path.length());

    std::vector<char> ReservedCharacters = {'!', '*', '\'', '(', ')', ';', ':', '@', '&', '=', '+', '$', ',', '/', '?', '#', '[', ']'};

    for(auto& c : path)
    {
        bool bAdded = false;
        for(auto& reserved : ReservedCharacters)
        {
            if(c == reserved)
            {
                encoded += EncodeChar(c);
                bAdded = true;
                break;
            }
        }
        if(!bAdded)
            encoded += c;
    }
    return encoded;

}
