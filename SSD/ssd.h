#include <string>
#include <vector>

using std::string;
using std::vector;

class SSD
{
public:
	void run(const string& commandStr);
	/*
	1. parser �����ؼ� �� �ؾ� �ϴ� �� �ľ�
	2. nand_txt ������ �� �о -> data ������ ����
	3. read/write ����
	*/

	void readSSDfile();
	string runReadCommand(int lba);
	void runWriteCommand(int lab, const string& value);

private:
	string data[100];
	vector<string> parsedCommand;
	// 0. ssd
	// 1. R/W
	// 2. lba
	// 3. value
};
