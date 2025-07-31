#include "logger.h"
#include <stdarg.h>
#include <iostream>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const char* funcName, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::string log_line = getLogMessage(funcName, fmt, args);
    va_end(args);

    // TODO: file IO
    std::cout << log_line << std::endl;
}

std::string Logger::getLogMessage(const char* funcName, const char* fmt, va_list args) {
    std::string funcStr(funcName);
    if (funcStr.length() < 30) {
        funcStr.append(30 - funcStr.length(), ' ');
    }
    char msgBuf[1024];
    vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);

    return getCurrentTimestamp() + " " + funcStr + " : " + msgBuf;
}

std::string Logger::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm localTime{};
    localtime_s(&localTime, &now);

    char buf[20];
    std::snprintf(buf, sizeof(buf), "[%02d.%02d.%02d %02d:%02d]",
        localTime.tm_year % 100,
        localTime.tm_mon + 1,
        localTime.tm_mday,
        localTime.tm_hour,
        localTime.tm_min);

    return std::string(buf);
}
