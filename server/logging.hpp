#pragma once

#include <string>
#include <utils.hpp>

class Logger
{
public:
    std::string filename;

    Logger() = default;

    Logger(std::string const& filename) : filename(filename){}

    Logger(Logger const& other)
    {
        filename = other.filename;
    }

    Logger& operator=(Logger const& other)
    {
        filename = other.filename;
        return *this;
    }

    ~Logger() = default;

    template <typename ...Args>
    void Log(std::string format,Args ... args)
    {
        FILE* file = fopen(filename.c_str(), "a+");
        if(!file)
            throw std::runtime_error(std::string("[Logger::Log]: error opening log file: ") + filename);
        format += "\n";

        std::string buf = string_format(format, args...);
        fwrite(buf.data(), sizeof(char), buf.size(), file);
        fclose(file);
    }

    void Log(std::string str)
    {
        FILE* file = fopen(filename.c_str(), "a+");
        if(!file)
            throw std::runtime_error(std::string("[Logger::Log]: error opening log file: ") + filename);
        str += "\n";

        fwrite(str.data(), sizeof(char), str.size(), file);
        fclose(file);
    }
};