#pragma once
#include <string>
#include <fstream>
#include <cstdarg>
#include <iomanip>
#include <cstdio>
#include <unordered_set>
#include "Exeptions.h"

namespace Log
{
enum Level : uint64_t
{
      debug = 0
    , info
    , warning
    , error
    , fatal
    , off
};

template<Level level>
inline char const* toStr()
{
    return "off";
}
template<>
inline char const* toStr<Level::debug>()
{
    return "Debug";
}
template<>
inline char const* toStr<Level::info>()
{
    return "Info";
}
template<>
inline char const* toStr<Level::warning>()
{
    return "Warning";
}
template<>
inline char const* toStr<Level::error>()
{
    return "Error";
}
template<>
inline char const* toStr<Level::fatal>()
{
    return "Fatal";
}

}

class Logger
{
    static char const* file;
    static std::fstream out;
    static bool isStarted;
    static Log::Level level;
    static char buffer[256];
public:
    static void start();
    static void stop();
    template<Log::Level logLevel>
    static void print(char const* , ... );
    static void setLevel(Log::Level const&);
    static void setLevel(std::string const& str);
};


template<Log::Level logLevel>
void Logger::print(char const* str, ...)
{
    if(!isStarted)
    {
        throw error::ExeptionBase<error::ErrorList::LogError>("Logger wasn't start yet");
    }
    if(logLevel < level)
    {
        return;
    }
    va_list arg;
    va_start(arg, str);
    for(char& i : buffer) i = '\0';
    vsprintf(buffer, str, arg);
    out << std::setfill(' ') << std::setw(7) << Log::toStr<logLevel>() << ": " << buffer << std::endl;
    out.flush();
}