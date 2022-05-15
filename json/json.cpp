#include "json.hpp"

#include "parser.hpp"

#include <fstream>
std::string ReadFile(std::string const& filename)
{
    std::fstream file(filename, std::ios::in);
    assert(file.is_open() && file.good());
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

Object JSON::Parse(std::string const& source)
{
    return Parser(source).Parse();
}

Object JSON::LoadFromFile(std::string const& filename)
{
    return JSON::Parse(ReadFile(filename));
}


std::string JSON::ToString(Object const& obj)
{
    return obj.Dump();
}