#include "ssd_command_parser.h"

class SSD
{
public:
	void run(const string& commandStr);
	void setParser(SSDCommandParser* parser);
	/*
	1. parser 수행해서 뭘 해야 하는 지 파악
	2. nand_txt 파일을 쭉 읽어서 -> data 변수에 저장
	3. read/write 수행
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
