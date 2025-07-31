#include "logger.h"
#include <stdarg.h>
#include <iostream>
#include <sstream>
#include "file_util.h"
#include <iomanip> 
#include <chrono>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const char* funcName, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    std::string log_line = getLogMessage(funcName, fmt, args);
    va_end(args);
#if _DEBUG
    std::cout << log_line << std::endl;
#endif
    rotateIfNeeded(log_line.size());
    writeToFile(log_line);
}

std::string Logger::getLogMessage(const char* funcName, const char* fmt, va_list args) {
    char msgBuf[1024];
    vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);

    std::ostringstream oss{};
    oss << getCurrentTimestamp() << " "
        << std::left << std::setw(30) << funcName 
        << " : " << msgBuf;

    return oss.str();
}

void Logger::writeToFile(const std::string& log_msg) {
    if (!FileUtil::directoryExists(LOG_DIR)) {
        bool ret = FileUtil::createDirectory(LOG_DIR);
        if (ret == false)
            return;
    }
    FileUtil::writeLine(LOG_FILE, log_msg, true);
}
void Logger::rotateIfNeeded(int size) {
    if (!FileUtil::directoryExists(LOG_DIR)) return;
    if (!FileUtil::fileExists(LOG_FILE)) return;
    if (FileUtil::getFileSize(LOG_FILE) + size < MAX_LOG_SIZE) return;

    // 날짜+시간 문자열 생성
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm;
    localtime_s(&local_tm, &t);

    char timestamp[64];
    std::strftime(timestamp, sizeof(timestamp), "until_%y%m%d_%Hh_%Mm_%Ss.log", &local_tm);

    std::string rotatedFile = LOG_DIR + "/" + timestamp;
    std::rename(LOG_FILE.c_str(), rotatedFile.c_str());
    FileUtil::clearFile(LOG_FILE);
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
