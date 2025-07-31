#include "ssd_command_parser.h"
#include "file_interface.h"
#include "nand_data.h"

#include <memory>

class SSD
{
public:
	explicit SSD(const std::string& nandPath = "../ssd_nand.txt",
		const std::string& outputPath = "../ssd_output.txt");


	void run(vector<string> commandVector);

	FileInterface& getNandFile();
	FileInterface& getOutputFile();
	bool readNandFile();
	bool writeNandFile();
	bool writeOutputFile(const string& str);

	// for unit test
	string getData(int lba) const;
	void writeData(int lba, const string& value);
	void clearData();

	void setParser(std::shared_ptr<SSDCommandParser> parser);

	vector<string> parsedCommand;

	//string data[100];
	NandData storage;

private:
	string runReadCommand(int lba);
	void runWriteCommand(int lba, const string& value);

	FileInterface outputFile;
	FileInterface nandFile;

	std::shared_ptr<SSDCommandParser> parser;

	static const int nandFileSize = 1200;
};
