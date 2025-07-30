#include "ssd_command_parser.h"

class SSD
{
public:
	void run(const string& commandStr);
	void setParser(SSDCommandParser* parser);
	/*
	1. parser �����ؼ� �� �ؾ� �ϴ� �� �ľ�
	2. nand_txt ������ �� �о -> data ������ ����
	3. read/write ����
	*/

	bool readSSDfile();
	bool writeSSDfile();

	string runReadCommand(int lba);
	void runWriteCommand(int lab, const string& value);

	vector<string> parsedCommand;
private:
	string data[100];
	SSDCommandParser* parser;
	// 0. ssd
	// 1. R/W
	// 2. lba
	// 3. value
};
