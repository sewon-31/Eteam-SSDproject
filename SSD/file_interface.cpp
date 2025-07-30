#include "file_interface.h"

FileInterface::FileInterface(const std::string& file) { 
	fileName = file;
	readPoint = 0;
}

bool FileInterface::fileOpen() {
	if (ssdNandFile.is_open())
		ssdNandFile.close();

	ssdNandFile.open(fileName, std::ios::in | std::ios::out);

	if (!ssdNandFile.is_open()) {
		std::cout << "[Debug] Creating new file: " << fileName << std::endl;

		std::ofstream create(fileName);
		create.close();

		ssdNandFile.open(fileName, std::ios::in | std::ios::out);
	}

	readPoint = 0;
	bool is_open = ssdNandFile.is_open();
	std::cout << "[Debug] fileOpen: is_open=" << is_open << std::endl;
	return is_open;
}

bool FileInterface::fileReadOneline(std::string& str) {
	if (!ssdNandFile.is_open()) return false;

	ssdNandFile.clear();
	ssdNandFile.seekg(readPoint, std::ios::beg);
	getline(ssdNandFile, str);
	readPoint = ssdNandFile.tellg();
	return ssdNandFile.good();
}
bool FileInterface::setReadPoint(unsigned point) {
	if (!ssdNandFile.is_open()) return false;

	readPoint = point;
	return ssdNandFile.good();
}

bool FileInterface::fileWriteOneline(const std::string str) {
	if (!ssdNandFile.is_open()) return false;

	ssdNandFile.seekg(0, std::ios::end);
	ssdNandFile << str << '\n';

	if (!ssdNandFile.good()) {
		std::cout << "[Error] write failed: fail=" << ssdNandFile.fail()
			<< ", bad=" << ssdNandFile.bad() << std::endl;
	}

	return ssdNandFile.good();
}

void FileInterface::fileClose() {
	if (ssdNandFile.is_open()) {
	    ssdNandFile.close();
	}
}

bool FileInterface::fileRemove() {
	std::ofstream file(fileName, std::ios::trunc);
	if (file.is_open()) {
		file.close();
		return true;
	}
	return false;
};
int FileInterface::checkSize() {
	std::ofstream file(fileName, std::ios::app);
	if (!file.is_open()) return 0;
	
	file.seekp(0, std::ios::end);
	int size = static_cast<int>(file.tellp());
	file.close();
	return size;
};
