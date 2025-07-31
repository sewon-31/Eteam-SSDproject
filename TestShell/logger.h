#pragma once
#include <string>
#include <vector>
class Logger {
public:
    static Logger& getInstance();
    void log(const char* funcName, const char* fmt, ...);

private:
    const std::string LOG_DIR = "../log";
    const std::string LOG_FILE = LOG_DIR + "/latest.log";
#if _DEBUG
    const int MAX_LOG_SIZE = 200;//for test: 200B, real: 10KB
#else
    const int MAX_LOG_SIZE = 10 * 1024;//for test: 200B, real: 10KB
#endif
    Logger() = default;
    std::string getCurrentTimestamp();
    std::string getLogMessage(const char* funcName, const char* fmt, va_list args);
    void writeToFile(const std::string& log_msg);
    void rotateIfNeeded(int size);
    std::vector<std::string> getLogFileList();
    void zipLogFile();
};