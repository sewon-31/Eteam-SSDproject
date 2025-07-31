#pragma once
#include <string>
class Logger {
public:
    static Logger& getInstance();
    void log(const char* funcName, const char* fmt, ...);

private:
    Logger() = default;
    std::string getCurrentTimestamp();
    std::string getLogMessage(const char* funcName, const char* fmt, va_list args);
};