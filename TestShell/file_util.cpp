#include "file_util.h"
#include <fstream>
#include <sys/stat.h>
#include <direct.h>

#define mkdir _mkdir

bool FileUtil::directoryExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) return false;
    return (info.st_mode & S_IFDIR);
}

bool FileUtil::createDirectory(const std::string& path) {
    if (directoryExists(path)) return true;
    return mkdir(path.c_str()) == 0;
}

bool FileUtil::fileExists(const std::string& path) {
    std::ifstream f(path.c_str());
    return f.good();
}

bool FileUtil::writeLine(const std::string& filePath, const std::string& line, bool append) {
    std::ofstream ofs;
    if (append)
        ofs.open(filePath.c_str(), std::ios::app);
    else
        ofs.open(filePath.c_str(), std::ios::trunc);

    if (!ofs.is_open()) return false;
    ofs << line << std::endl;
    return ofs.good();
}

bool FileUtil::readLine(const std::string& filePath, std::string& lineOut) {
    std::ifstream ifs(filePath.c_str());
    if (!ifs.is_open()) return false;
    if (!std::getline(ifs, lineOut)) {
        return false;
    }
    return ifs.good();
}

bool FileUtil::clearFile(const std::string& filePath) {
    std::ofstream ofs(filePath.c_str(), std::ios::trunc);
    return ofs.is_open();
}

bool FileUtil::writeAllLines(const std::string& filePath, const std::vector<std::string>& lines, bool append) {
    std::ofstream ofs;
    if (append)
        ofs.open(filePath.c_str(), std::ios::app);
    else
        ofs.open(filePath.c_str(), std::ios::trunc);

    if (!ofs.is_open()) return false;

    for (const auto& line : lines) {
        ofs << line << std::endl;
        if (!ofs.good()) return false;
    }
    return true;
}

bool FileUtil::readAllLines(const std::string& filePath, std::vector<std::string>& linesOut) {
    std::ifstream ifs(filePath.c_str());
    if (!ifs.is_open()) return false;

    std::string line;
    while (std::getline(ifs, line)) {
        linesOut.push_back(line);
    }
    return ifs.eof();
}
