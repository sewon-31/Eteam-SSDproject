#pragma once
#include <string>
#include <fstream>
#include <iostream>

class FileInterface {
public:
	bool fileOpen(const std::string& fileName);
	bool fileReadOneline(std::string& str);
	bool setReadPoint(unsigned point);
	bool fileWriteOneline(const std::string str);
	void fileClose();
	bool fileRemove(const std::string& fileName);
	int checkSize(const std::string& fileName);
private:
	std::fstream ssdNandFile;
	unsigned int readPoint;
};