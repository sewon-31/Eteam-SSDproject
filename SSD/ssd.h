#include "ssd_command_parser.h"
#include "file_interface.h"

class TEST_CMD {
public:
	string op[6];
	int lba[6];
	int size[6];
	string data[6];

};

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
	int reduceCMDBuffer(TEST_CMD in, TEST_CMD& out);
	vector<string> parsedCommand;

	string data[100];

private:
	int reduceCMDBufferDisplay(TEST_CMD in);
	int reduceCMDBufferSeqCMD(TEST_CMD in, TEST_CMD& out);
	int reduceCMDBufferNonSeqCMD(TEST_CMD in, TEST_CMD& out, int newCMDCount);

	FileInterface outputFile;
	FileInterface nandFile;
	SSDCommandParser* parser = nullptr;
	static const int nandFileSize = 1200;
	static const int maxLbaNum = 100;
};
