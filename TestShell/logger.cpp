#include "logger.h"
#include <stdarg.h>
#include <iostream>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const char* funcName, const char* fmt, ...) {
    std::time_t now = std::time(nullptr);
    std::tm localTime{};
    localtime_s(&localTime, &now);

    char timeBuf[20];
    std::snprintf(timeBuf, sizeof(timeBuf), "[%02d.%02d.%02d %02d:%02d]",
        localTime.tm_year % 100,
        localTime.tm_mon + 1,
        localTime.tm_mday,
        localTime.tm_hour,
        localTime.tm_min);

    std::string funcStr = std::string(funcName);
    if (funcStr.length() < 30) {
        funcStr.append(30 - funcStr.length(), ' ');
    }

    char msgBuf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
    va_end(args);

    std::cout << timeBuf << " " << funcStr << " : " << msgBuf << std::endl;
}
