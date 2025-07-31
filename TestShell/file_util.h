#pragma once

#include <string>
#include <vector>

class FileUtil {
public:
    static bool directoryExists(const std::string& path);
    static bool createDirectory(const std::string& path);
    static bool fileExists(const std::string& path);
    static bool writeLine(const std::string& filePath, const std::string& line, bool append = true);
    static bool readLine(const std::string& filePath, std::string& lineOut);
    static bool clearFile(const std::string& filePath);
    static bool writeAllLines(const std::string& filePath, const std::vector<std::string>& lines, bool append = false);
    static bool readAllLines(const std::string& filePath, std::vector<std::string>& linesOut);
    static size_t getFileSize(const std::string& filePath);
};
