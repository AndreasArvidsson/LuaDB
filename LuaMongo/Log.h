#pragma once

#ifdef DEBUG

#include <cstring>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__))
#define LOG_INFO(str, ...)      Log::log("INFO", __FILENAME__, __LINE__, str, ##__VA_ARGS__)
#define LOG_WARN(str, ...)      Log::log("WARNING", __FILENAME__, __LINE__, str, ##__VA_ARGS__)
#define LOG_ERROR(str, ...)     Log::log("ERROR", __FILENAME__, __LINE__, str, ##__VA_ARGS__)
#define LOG_POSITION()          Log::log("POSITION", __FILENAME__, __LINE__, "")

class Log {
public:    
    static void log(const char* type, const char* file, unsigned int line, const char *str, ...);

};

#endif

