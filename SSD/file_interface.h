#pragma once
#include <string>
#include <fstream>
#include <iostream>

class FileInterface {
public:
	FileInterface(const std::string& file);
	bool fileOpen();
	bool fileReadOneline(std::string& str);
	bool setReadPoint(unsigned point);
	bool fileWriteOneline(const std::string str);
	void fileClose();
	bool fileRemove();
	int checkSize();
private:
	std::fstream ssdNandFile;
	unsigned int readPoint;
	std::string fileName;
};