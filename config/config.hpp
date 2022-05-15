#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../filesystem/fs.hpp"
#include "../utils.hpp"
#include "../debug.hpp"

#include "../json/json.hpp"



#include <map>

class Config
{
public:
  typedef struct {
    std::string key, value;
  }entry_t;

  Config();

  Config(std::string const& file);

  Config(const Config& other);

  Config& operator=(const Config& other);

  bool exists(std::string const& key) const;

  Object& Get(std::string const& key);

  Object const& Get(std::string const& key) const;

  void Set(std::string const& key, Object const& value);

  Object& operator[](std::string const& key);


private:

public:
  std::string m_filename;
private:
  Object m_object;

};


#endif
