#include "file_interface.h"

bool FileInterface::fileOpen(const std::string& fileName) {
	ssd_nand_file.open(fileName, std::ios::in | std::ios::out | std::ios::app);
	read_point = 0;
	return ssd_nand_file.is_open();
}
bool FileInterface::fileReadOneline(std::string& str) {
	if (!ssd_nand_file.is_open()) return false;

	ssd_nand_file.clear();
	ssd_nand_file.seekg(read_point, std::ios::beg);
	getline(ssd_nand_file, str);
	read_point = ssd_nand_file.tellg();
	return ssd_nand_file.good();
}
bool FileInterface::setReadPoint(unsigned point) {
	if (!ssd_nand_file.is_open()) return false;

	read_point = point;
	return ssd_nand_file.good();
}
bool FileInterface::fileWriteOneline(const std::string str) {
	if (!ssd_nand_file.is_open()) return false;

	std::string write_str = str + "\n";

	ssd_nand_file.seekg(0, std::ios::end);
	ssd_nand_file.write(write_str.c_str(), write_str.length());
	return ssd_nand_file.good();
}
void FileInterface::fileClose() {
	ssd_nand_file.close();
}
bool FileInterface::fileRemove(const std::string& fileName) {
	std::ofstream file(fileName, std::ios::trunc);
	if (file.is_open()) {
		file.close();
		return true;
	}
	return false;
};