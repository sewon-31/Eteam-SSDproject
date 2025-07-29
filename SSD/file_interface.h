#pragma once
#include <string>
#include <fstream>
#include <iostream>

class FileInterface {
public:
	bool fileOpen(const std::string& fileName);
	bool fileReadOneline(std::string& str);
	bool setRead_Point(unsigned point);
	bool fileWriteOneline(const std::string str);
	void fileClose();
	bool fileRemove(const std::string& fileName);
private:
	std::fstream ssd_nand_file;
	unsigned int read_point;
};