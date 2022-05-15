#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>

/* this file contains debuggig macros and other essentials */

inline void log(char const* message, char const* file, size_t line)
{
  printf("[LOG]: %s %s:%zu\n", message, file, line);
}

inline void log(char const* message)
{
  printf("[LOG]: %s\n", message);
}

inline void error(char const* message, char const* file, size_t line)
{
  printf("[ERROR]: %s at %s:%zu\n", message, file, line);
}

inline void error(char const* message)
{
  printf("[ERROR]: %s\n", message);
}

inline void error_fatal(char const* message)
{
  printf("[FATAL]: %s\n", message);
  exit(1);
}

template <typename T>
inline void debug(char const* message, T const& value)
{
  std::cout << "[DEBUG]: " << message << ": " << value << "\n";
}

#define DEBUG(variable) debug(#variable, variable);
#define LOG(message) log(message);

#define ASSERT(statement) ((statement) ? (void)0 : error(#statement,__FILE__,__LINE__));
#define ERROR(msg) error(msg);

#define ERROR_FATAL(msg) error_fatal(msg);

#endif
