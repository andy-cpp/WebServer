#include "config.hpp"

Config::Config()
{

}

Config::Config(std::string const& filename)
{
  m_filename = filename;
  m_object = JSON::LoadFromFile(filename);
}

Config::Config(const Config& other)
{
  m_filename = other.m_filename;
  m_object = other.m_object;
}

Config& Config::operator=(const Config& other)
{
  m_filename = other.m_filename;
  m_object = other.m_object;

  return *this;
}



bool Config::exists(std::string const& key) const
{
  return m_object.ToDictionary().count(key) != 0;
}


Object& Config::Get(std::string const& key)
{
  if(m_object.ToDictionary().count(key) == 0)
    throw std::runtime_error(std::string("[Config::Get]: could not get ") + key);
  
  return m_object.Get(key);
}

Object const& Config::Get(std::string const& key) const
{
  if(m_object.ToDictionary().count(key) == 0)
    throw std::runtime_error(std::string("[Config::Get]: could not get ") + key);
  return m_object.Get(key);
}

void Config::Set(std::string const& key, Object const& value)
{
  m_object.Set(key, value);
}



Object& Config::operator[](std::string const& key)
{
  if(m_object.ToDictionary().count(key) == 0)
    throw std::runtime_error(std::string("[Config::Get]: could not get ") + key);
  

  return m_object.Get(key);
}
