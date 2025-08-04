#include "file_interface.h"
#include <fstream>

namespace fs = std::filesystem;

bool FileInterface::readLine(const std::string& filePath, std::string& lineOut) {
	std::ifstream ifs(filePath);
	return ifs && static_cast<bool>(std::getline(ifs, lineOut));
}

bool FileInterface::readAllLines(const std::string& filePath, std::vector<std::string>& linesOut) {
	std::ifstream ifs(filePath);
	if (!ifs) return false;

	std::string line;
	while (std::getline(ifs, line)) {
		linesOut.push_back(line);
	}
	return true;
}

bool FileInterface::writeLine(const std::string& filePath, const std::string& line, bool append) {
	std::ofstream ofs(filePath, append ? std::ios::app : std::ios::trunc);
	if (!ofs) return false;
	ofs << line << '\n';
	return ofs.good();
}

bool FileInterface::writeAllLines(const std::string& filePath, const std::vector<std::string>& lines, bool append) {
	std::ofstream ofs(filePath, append ? std::ios::app : std::ios::trunc);
	if (!ofs) return false;

	for (const auto& line : lines) {
		ofs << line << '\n';
		if (!ofs) return false;
	}
	return true;
}

bool FileInterface::clearFile(const std::string& filePath) {
	std::ofstream ofs(filePath, std::ios::trunc);
	return ofs.is_open();
}

size_t FileInterface::getFileSize(const std::string& filePath) {
	std::error_code ec;
	auto size = fs::file_size(filePath, ec);
	return ec ? 0 : static_cast<size_t>(size);
}

bool FileInterface::fileExists(const std::string& filePath) {
	return fs::is_regular_file(filePath);
}

bool FileInterface::renameFile(const std::string& oldPath, const std::string& newPath) {
	try {
		fs::rename(oldPath, newPath);
		return true;
	}
	catch (...) {
		return false;
	}
}

bool FileInterface::removeFile(const std::string& filePath) {
	try {
		return fs::remove(filePath);
	}
	catch (...) {
		return false;
	}
}

bool FileInterface::createDirectory(const std::string& dirPath) {
	try {
		return fs::create_directories(dirPath);
	}
	catch (...) {
		return false;
	}
}

bool FileInterface::directoryExists(const std::string& dirPath) {
	return fs::exists(dirPath) && fs::is_directory(dirPath);
}

bool FileInterface::clearDirectory(const std::string& dirPath) {
	try {
		for (const auto& entry : fs::directory_iterator(dirPath)) {
			if (fs::is_regular_file(entry)) {
				fs::remove(entry.path());
			}
		}
		return true;
	}
	catch (...) {
		return false;
	}
}
