#include "ssd_command_parser.h"
#include "file_interface.h"

class SSD
{
public:
	void run(const string& commandStr);
	void setParser(SSDCommandParser* parser);

	bool readNandFile();
	bool writeNandFile();
	void writeOutputFile(const string& str);

	string runReadCommand(int lba);
	void runWriteCommand(int lba, const string& value);

	void clearData();
	string getData(int lba) const;

	vector<string> parsedCommand;

	string data[100];
	string data_temp[100];
	FileInterface* outputFile;
	FileInterface nandFile = { "ssd_nand.txt" };
private:

	SSDCommandParser* parser = nullptr;

};