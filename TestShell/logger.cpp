#include "logger.h"
#include <stdarg.h>
#include <iostream>
#include <sstream>
#include "file_util.h"
#include <iomanip> 
#include <chrono>
#include <io.h>

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

void Logger::zipLogFile() {
    auto files = getLogFileList();
    if (files.empty()) return;
    auto oldFileName = LOG_DIR + "/" + files.at(0);

    const std::string oldExt = ".log";
    const std::string newExt = ".zip";
    std::string newFileName = oldFileName.substr(0, oldFileName.size() - oldExt.size()) + newExt;
    std::rename(oldFileName.c_str(), newFileName.c_str());
}

void Logger::rotateIfNeeded(int size) {
    if (!FileUtil::directoryExists(LOG_DIR)) return;
    if (!FileUtil::fileExists(LOG_FILE)) return;
    if (FileUtil::getFileSize(LOG_FILE) + size < MAX_LOG_SIZE) return;
    zipLogFile();
    auto backupFile = getBackupLogFileName();
    if (std::rename(LOG_FILE.c_str(), backupFile.c_str()) != 0) {
        std::cerr << "Failed to rename log file for rotation\n";
        return;
    }
    FileUtil::clearFile(LOG_FILE);
}

std::string Logger::getBackupLogFileName()
{
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm;
    localtime_s(&local_tm, &t);

    char timestamp[64];
    std::strftime(timestamp, sizeof(timestamp), "until_%y%m%d_%Hh_%Mm_%Ss.log", &local_tm);
    return LOG_DIR + "/" + timestamp;
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

std::vector<std::string> Logger::getLogFileList() {
    struct _finddata_t fileinfo;
    intptr_t hFile = _findfirst((LOG_DIR + "/until_*.log").c_str(), &fileinfo);
    std::vector<std::string> files;
    if (hFile != -1) {
        do {
            files.push_back(fileinfo.name);
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
    return files;
}