#pragma once
#include <string>
#include <fstream>
#include <iostream>

class FileInterface
{
public:
	FileInterface(const std::string& file);

	bool fileOpen();
	void fileClose();
	bool fileClear();

	bool fileReadOneline(std::string& str);
	bool setReadPoint(unsigned point);

	bool fileWriteOneline(const std::string str);

	int checkSize();

private:
	std::string fileName;
	std::fstream file;

	unsigned int readPoint;
};