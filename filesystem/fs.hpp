#ifndef FS_HPP
#define FS_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace FS
{
  bool IsFile(std::string const& path);

  bool IsDirectory(std::string const& path);

  std::vector<std::string> GetNamesInDirectory(std::string const& dir);

  bool Exists(std::string const& path);

  size_t FileSize(std::string const& filepath);

  long long write(std::string const& filename, std::string const& data);

  long long read(std::string const& filename, std::string& buffer);

  long long read(std::string const& filename, std::ostringstream& buffer);
}



#endif
