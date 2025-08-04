#pragma once
#include <string>
#include <vector>
#include <filesystem>

class FileInterface {
public:
	// Basic file ops
	static bool readLine(const std::string& filePath, std::string& lineOut);
	static bool readAllLines(const std::string& filePath, std::vector<std::string>& linesOut);
	static bool writeLine(const std::string& filePath, const std::string& line, bool append = true);
	static bool writeAllLines(const std::string& filePath, const std::vector<std::string>& lines, bool append = false);
	static bool clearFile(const std::string& filePath);
	static size_t getFileSize(const std::string& filePath);

	// Filesystem util
	static bool fileExists(const std::string& filePath);
	static bool renameFile(const std::string& oldPath, const std::string& newPath);
	static bool removeFile(const std::string& filePath);
	static bool createDirectory(const std::string& dirPath);
	static bool directoryExists(const std::string& dirPath);
	static bool clearDirectory(const std::string& dirPath);
};
	