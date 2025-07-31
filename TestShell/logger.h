#pragma once
#include <string>
class Logger {
public:
    static Logger& getInstance();
    void log(const char* funcName, const char* fmt, ...);

private:
    const std::string logDir = "../log";
    const std::string logFile = logDir + "/latest.log";
    Logger() = default;
    std::string getCurrentTimestamp();
    std::string getLogMessage(const char* funcName, const char* fmt, va_list args);
    void writeToFile(const std::string& log_msg);
};