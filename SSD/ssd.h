#include "ssd_command_parser.h"
#include "file_interface.h"

class SSD
{
public:
	explicit SSD(const std::string& nandPath = "../ssd_nand.txt",
		const std::string& outputPath = "../ssd_output.txt");


	void run(const string& commandStr);
	void setParser(SSDCommandParser* parser);

	FileInterface& getNandFile();
	FileInterface& getOutputFile();
	bool readNandFile();
	bool writeNandFile();
	bool writeOutputFile(const string& str);

	string runReadCommand(int lba);
	void runWriteCommand(int lba, const string& value);

	void clearData();
	string getData(int lba) const;

	vector<string> parsedCommand;

	string data[100];

private:
	FileInterface outputFile;
	FileInterface nandFile;
	SSDCommandParser* parser = nullptr;
	static const int nandFileSize = 1200;
	static const int maxLbaNum = 100;
};
