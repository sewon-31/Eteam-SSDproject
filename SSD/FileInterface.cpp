#include <string>
#include <fstream>

class FileInterface {
public:
	bool fileOpen(const std::string &fileName) {
		fd_nand.open(fileName);
		return !fd_nand.fail();
	};
	std::string fileRead(std::string fileName) {
		return "";
	};
	bool fileWrite(std::string str) {
		fd_nand.write(str.c_str(), str.length());
		return true;
	};

private:
	std::ofstream fd_nand;
};
