#include <string>
#include <vector>

using std::string;
using std::vector;

class SSD
{
public:
	void run(const string& commandStr);
	/*
	1. parser 수행해서 뭘 해야 하는 지 파악
	2. nand_txt 파일을 쭉 읽어서 -> data 변수에 저장
	3. read/write 수행
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
