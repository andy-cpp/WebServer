#include <errno.h>
#include <iostream>

#include "utils.hpp"

#include "server/server.hpp"
#include "debug.hpp"

#include <openssl/ssl3.h>
#include <thread>

#include <system/system.hpp>
#include <stdlib.h>
#include <limits.h>
#include <filesystem/fs.hpp>
#include <sys/stat.h>

void testing()
{
  FS::GetNamesInDirectory(".");

  struct stat info;
  if(stat("./test.txt", &info) != 0){
    printf("oops couldn't stat ./test.txt\n");
    exit(1);
  };

  

}



int main()
{
  _SSL::Init();

  std::vector<Server*> Servers;

  /* load all configs */
  std::vector<std::string> Configs = Server::GetConfigFiles();

  /* Start all servers */
  for(auto const& config_name : Configs)
  {
    Server* pServer = new Server(config_name);
    if(!pServer->VerifyConfig())
      continue;
    Servers.push_back(pServer);

    pServer->InitDetails();
    pServer->InitSocket();

    std::thread(&Server::Start, pServer).detach();

  }

  /* wait until all servers close */
  while(Servers.size() > 0)
  {
    usleep(100000);
  }
}
