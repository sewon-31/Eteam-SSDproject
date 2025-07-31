#pragma once

#include <string>

class FileUtil {
public:
    static bool directoryExists(const std::string& path);
    static bool createDirectory(const std::string& path);
    static bool fileExists(const std::string& path);
    static bool writeLine(const std::string& filePath, const std::string& line, bool append = true);
    static bool readLine(const std::string& filePath, std::string& lineOut);
    static bool clearFile(const std::string& filePath);
};
