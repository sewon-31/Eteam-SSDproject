#include <string>
#include <fstream>
#include <iostream>

class FileInterface {
public:
	bool fileOpen(const std::string &fileName) {
		fd_nand.open(fileName);
		return !fd_nand.fail();
	};
	bool fileRead(std::string &str) {
		fd_nand.seekg(0, 0);
		getline(fd_nand, str);
		return true;
	};
	bool fileWrite(std::string str) {
		fd_nand.write(str.c_str(), str.length());
		return true;
	};

private:
	std::fstream fd_nand;
};
