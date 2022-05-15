#include "fs.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

#include "../utils.hpp"

#include <stdexcept>

bool FS::IsFile(std::string const& path)
{
  struct stat path_stat;
  if(stat(path.c_str(), &path_stat) != 0)
    return false;
  return S_ISREG(path_stat.st_mode);
}

bool FS::IsDirectory(std::string const& path)
{
  struct stat path_stat;
  if(stat(path.c_str(), &path_stat) != 0)
    return false;
  return S_ISDIR(path_stat.st_mode);
}

#include <dirent.h>
std::vector<std::string> FS::GetNamesInDirectory(std::string const& dirname)
{
  DIR* d = opendir(dirname.c_str());
  struct dirent* dir;
  if(!d)
    return {};

  std::vector<std::string> names;
  while((dir = readdir(d)) != 0)
  {
    if(strcmp(dir->d_name, ".") == 0) continue;
    if(strcmp(dir->d_name, "..") == 0) continue;

    names.push_back(dir->d_name);
  }
  closedir(d);
  return names;
  
}


size_t FS::FileSize(std::string const& filepath)
{
  struct stat st;
  if(stat(filepath.c_str(), &st) != 0)
    throw std::runtime_error("[FS::FileSize]: error getting file size");
  return st.st_size;
}


bool FS::Exists(std::string const& path)
{
  struct stat path_stat;
  return stat(path.c_str(), &path_stat) != -1;
}

long long FS::write(std::string const& filename, std::string const& data)
{
  FILE* file = fopen(filename.c_str(), "w");
  if(file == 0)
    return -1;

  long long bytes = fwrite(data.c_str(), sizeof(char), data.size(), file);
  fclose(file);
  return bytes;
}

long long FS::read(std::string const& filename, std::string& buffer)
{
  FILE* file = fopen(filename.c_str(), "r");
  if(file == 0)
    return -1;

  char* buf = (char*)malloc(1024);
  if(buf == 0)
    throw std::runtime_error("Nullptr exception in FS::read");
  long long bytes_read = 0;
  while(true)
  {
      memset(buf, 0, 1024);
      size_t bytes = fread(buf, sizeof(char),1024, file);
      if(bytes < 0)
        break;
      bytes_read += bytes;
      buffer.append(std::string(buf, bytes));

      if(bytes != 1024)
        break;
  }
  free(buf);
  fclose(file);
  return bytes_read;
}


long long FS::read(std::string const& filename, std::ostringstream& buffer)
{
  FILE* file = fopen(filename.c_str(), "r");
  if(file == 0)
    return -1;

  char* buf = (char*)malloc(1024);
  if(buf == 0)
    throw std::runtime_error("Nullptr exception in FS::read");
  long long bytes_read = 0;
  while(true)
  {
      memset(buf, 0, 1024);
      size_t bytes = fread(buf, sizeof(char),1024, file);
      if(bytes < 0)
        break;
      bytes_read += bytes;
      buffer << buf;

      if(bytes != 1024)
        break;
  }
  free(buf);
  fclose(file);
  return bytes_read;

}
