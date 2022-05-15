#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <string>
#include <vector>

#include <iostream>
#include <stdexcept>
#include <array>
#include <memory>

#include "../filesystem/fs.hpp"
#include "../utils.hpp"



std::string exec(const char* cmd);


struct ProgramContext
{
    FILE* stdin, *stdout, *stderr;
    int pid;
    int status; // return value
};

ProgramContext ExecuteProgram(std::string const& filename, std::vector<std::string> args, std::vector<std::string> env);



#endif /* end of include guard: SYSTEM_HPP */
