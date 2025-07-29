#include <string>
#include <fstream>
#include <iostream>

class FileInterface {
public:
	bool fileOpen(const std::string &fileName) {
		fd_nand.open(fileName);
		return !fd_nand.fail();
	};
	bool fileReadOneline(std::string &str) {
		fd_nand.seekg(read_point, std::ios::beg);
		getline(fd_nand, str);
		read_point = fd_nand.tellg();
		return true;
	};
	bool fileWriteOneline(std::string str) {
		fd_nand.seekg(0, std::ios::end);
		fd_nand.write(str.c_str(), str.length());
		return true;
	};
	bool fileClose() {
		fd_nand.close();
		return true;
	};
private:
	std::fstream fd_nand;
	unsigned int read_point = 0;
};
