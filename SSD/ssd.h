#include "ssd_command_parser.h"
#include "file_interface.h"

class SSD
{
public:
	void run(const string& commandStr);
	void setParser(SSDCommandParser* parser);

	void readNandFile();
	void writeNandFile();
	void writeOutputFile(const string& str);

	string runReadCommand(int lba);
	void runWriteCommand(int lba, const string& value);

	void clearData();
	string getData(int lba) const;

	vector<string> parsedCommand;

private:
	string data[100];

	SSDCommandParser* parser = nullptr;

	FileInterface* outputFile;
	FileInterface* nandFile;
};
