#include "file_util.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

bool FileUtil::directoryExists(const std::string& path) {
	return fs::is_directory(path);
}

bool FileUtil::createDirectory(const std::string& path) {
	if (directoryExists(path)) return true;
	return fs::create_directories(path);
}

bool FileUtil::fileExists(const std::string& path) {
	return fs::is_regular_file(path);
}

bool FileUtil::writeLine(const std::string& filePath, const std::string& line, bool append) {
	std::ofstream ofs(filePath, append ? std::ios::app : std::ios::trunc);
	if (!ofs) return false;

	ofs << line << '\n';
	return ofs.good();
}

bool FileUtil::readLine(const std::string& filePath, std::string& lineOut) {
	std::ifstream ifs(filePath);
	return ifs && static_cast<bool>(std::getline(ifs, lineOut));
}

bool FileUtil::clearFile(const std::string& filePath) {
	std::ofstream ofs(filePath, std::ios::trunc);
	return ofs.is_open();
}

bool FileUtil::writeAllLines(const std::string& filePath, const std::vector<std::string>& lines, bool append) {
	std::ofstream ofs(filePath, append ? std::ios::app : std::ios::trunc);
	if (!ofs) return false;

	for (const auto& line : lines) {
		ofs << line << '\n';
		if (!ofs) return false;
	}
	return true;
}

bool FileUtil::readAllLines(const std::string& filePath, std::vector<std::string>& linesOut) {
	std::ifstream ifs(filePath);
	if (!ifs) return false;

	std::string line;
	while (std::getline(ifs, line)) {
		linesOut.push_back(line);
	}
	return true;
}

size_t FileUtil::getFileSize(const std::string& filePath) {
	std::error_code ec;
	const auto size = fs::file_size(filePath, ec);
	return ec ? 0 : static_cast<size_t>(size);
}
