#include "Object.hpp"

#include <memory>
#include <string>
#include <stdexcept>

template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    auto buf = std::make_unique<char[]>( size );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

std::string ObjectToString(Object const& obj, unsigned level = 0, bool skipstart = false);

void Object::Set(Object const& value)
{
    *this = value;
}

void Object::Set(std::string const& key, Object const& value)
{
    this->ToDictionary().insert({key, value});
}

Object& Object::Get(std::string const& key)
{
    return this->ToDictionary().at(key);
}

Object const& Object::Get(std::string const& key) const
{
    return this->ToDictionary().at(key);
}



std::string ArrayToString(Object::array_t const& array, unsigned level = 0, bool skipstart = false)
{
    std::string buffer = ((!skipstart) ? std::string(level * 4, ' ') : "") +  "[\n";
    
    for(unsigned index = 0; index < array.size(); ++index)
    {
        Object const& obj = array[index];
        //if(index == array.size() - 1) // last iteration
        buffer += string_format("%s%s%c\n", std::string((level + 1) * 4, ' ').c_str(), ObjectToString(obj, level + 1, true).c_str(), (index == array.size() - 1) ? ' ' : ',');

    }
    printf("level: %d\n", level);
    buffer += std::string(level * 4, ' ') + "]";
    return buffer;
}

std::string MapToString(Object::dictionary_t const& obj, unsigned level, bool skipstart)
{
    std::string buffer = ((!skipstart) ? std::string(level * 4, ' ') : "") +  "{\n";

    for(auto it = obj.begin(); it != obj.end(); ++it)
    {
        auto entry = *it;
        
        buffer += string_format("%s\"%s\": %s%c\n", std::string((level + 1) * 4, ' ').c_str(), entry.first.c_str(), ObjectToString(entry.second, level + 1, true).c_str(), (it == --obj.end() ? ' ' : ','));
    }

    buffer += std::string(level * 4, ' ') + "}";
    return buffer;
}

std::string ObjectToString(Object const& obj, unsigned level, bool skipstart)
{
    switch(obj.m_type)
    {
    case Object::Type::NUMBER:
        return std::to_string(obj.ToNumber());
    case Object::Type::STRING:
        return "\"" + obj.ToString() + "\"";
    case Object::Type::NULL_:
        return "null";
    case Object::Type::BOOL:
        return obj.ToBool() ? "true": "false";
    case Object::Type::ARRAY:
        // Call array to string
        return ArrayToString(obj.ToArray(), level, skipstart);
    case Object::Type::OBJECT:
        return MapToString(obj.ToDictionary(), level, skipstart);
    }
    throw std::runtime_error("[ObjectToString]: could not turn object into string");
}

std::string Object::Dump() const
{
    return ObjectToString(*this, 0);
}