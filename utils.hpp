#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <limits.h>
using std::string, std::vector;

std::string HTMLEncode(std::string const& data);

std::string HTMLDecode(std::string const& data);


inline std::string GetRealPath(std::string const& relativepath)
{
  char cbuf[PATH_MAX + 1] = {0};
  realpath(relativepath.c_str(), cbuf);
  return std::string(cbuf);
}

static char const** Vector2Array(std::vector<std::string> const& vector, bool bNullTerminated = false)
{
    unsigned size = vector.size() + (int)bNullTerminated;
    char const** ptr = new char const*[size];
    if(ptr == 0){
        throw std::runtime_error("[Vector2Array]: allocation error");
    }

    for(unsigned index = 0; index < vector.size(); ++index)
    {
        ptr[index] = vector[index].c_str();
    }

    if(bNullTerminated){
        ptr[size - 1] = 0;
    }

    return ptr;
}

inline std::vector<std::string> split(std::string const& str, std::string const& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

#include <memory>
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] ); 
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

inline bool startswith(std::string const& str, std::string const& startstring)
{
  return str.find(startstring) == 0;
}

inline bool endswith(std::string const& string, std::string const& endstr)
{
  if(endstr.size() > string.size())
    return false;

  std::string substr = string.substr(string.size() - endstr.size());

  return substr == endstr;
}

inline size_t strsize(std::ostringstream& ss)
{

  ss.seekp(0, std::ios::end);
  size_t size = ss.tellp();
  ss.seekp(0, std::ios::beg);
  return size;
}

inline std::string Vector2String(std::vector<std::string> const& vec)
{
  std::string retvalue;

  for(auto const& e : vec)
  {
    retvalue += e + " ";
  }
  retvalue.pop_back();
  return retvalue;
}

inline bool contains(std::string const& string, std::string const& contains_str)
{
  return string.find(contains_str) != std::string::npos;
}
