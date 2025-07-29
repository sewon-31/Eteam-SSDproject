#include "file_interface.h"

bool FileInterface::fileOpen(const std::string& fileName) {
	ssd_nand_file.open(fileName);
	read_point = 0;
	return !ssd_nand_file.fail();
}
bool FileInterface::fileReadOneline(std::string& str) {
	ssd_nand_file.seekg(read_point, std::ios::beg);
	getline(ssd_nand_file, str);
	read_point = ssd_nand_file.tellg();
	return !ssd_nand_file.fail();
}
bool FileInterface::setRead_Point(unsigned point) {
	read_point = point;
	return !ssd_nand_file.fail();
}
bool FileInterface::fileWriteOneline(const std::string str) {
	std::string write_str = str + "\n";
	ssd_nand_file.seekg(0, std::ios::end);
	ssd_nand_file.write(str.c_str(), str.length());
	return !ssd_nand_file.fail();
}
void FileInterface::fileClose() {
	ssd_nand_file.close();
}