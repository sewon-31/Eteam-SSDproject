#include "file_interface.h"

FileInterface::FileInterface(const std::string& file) 
{ 
	fileName = file;
	readPoint = 0;
}

bool 
FileInterface::fileOpen() 
{
	if (file.is_open())
		file.close();

	file.open(fileName, std::ios::in | std::ios::out);

	if (!file.is_open()) {
		std::ofstream(fileName).close();
		file.open(fileName, std::ios::in | std::ios::out);
	}

	readPoint = 0;

	return file.is_open();
}

void FileInterface::fileClose() 
{
	if (file.is_open()) {
	    file.close();
	}
}

bool 
FileInterface::fileClear() 
{
	std::ofstream file(fileName, std::ios::trunc);
	if (file.is_open()) {
		file.close();
		return true;
	}
	return false;
}

bool 
FileInterface::fileReadOneline(std::string& str) 
{
	if (!file.is_open()) return false;

	file.clear();
	file.seekg(readPoint, std::ios::beg);
	getline(file, str);
	readPoint = static_cast<unsigned>(file.tellg());

	return file.good();
}

bool 
FileInterface::setReadPoint(unsigned point) 
{
	if (!file.is_open()) return false;

	readPoint = point;
	return file.good();
}

bool 
FileInterface::fileWriteOneline(const std::string str) 
{
	if (!file.is_open()) return false;

	file.seekp(0, std::ios::end);
	file << str << '\n';

	if (!file.good()) {
		std::cout << "[Error] write failed: fail=" << file.fail()
			<< ", bad=" << file.bad() << std::endl;
	}

	return file.good();
}

int
FileInterface::checkSize() 
{
	std::ofstream file(fileName, std::ios::app);

	if (file.is_open())
		file.close();

	file.open(fileName, std::ios::in | std::ios::out);

	if (!file.is_open()) {
		std::ofstream(fileName).close();
		file.open(fileName, std::ios::in | std::ios::out);
	}
	
	file.seekp(0, std::ios::end);
	int size = static_cast<int>(file.tellp());
	file.close();
	return size;
}
