#include "file_interface.h"

bool FileInterface::fileOpen() {
	ssdNandFile.open(fileName, std::ios::in | std::ios::out | std::ios::app);
	readPoint = 0;
	return ssdNandFile.is_open();
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
	return ssdNandFile.good();
}
void FileInterface::fileClose() {
	ssdNandFile.close();
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
	int position = 0;

	if (file.is_open()) {
		file.seekp(0, std::ios::end);
		position = file.tellp();
		file.close();
		return position;
	}
	return 0;
};