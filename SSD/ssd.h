#pragma once
#include "ssd_command_builder.h"
#include "nand_data.h"
#include "command.h"
#include "command_buffer.h"
#include <memory>
#include <string>
#include <vector>

using std::string;

class SSD
{
public:
	explicit SSD(
		const string& nandPath = "../ssd_nand.txt",
		const string& outputPath = "../ssd_output.txt");

	void run(vector<string> commandVector);
	bool updateOutputFile(const string& result);

	// for unit test
	string getData(int lba) const;
	void writeData(int lba, const string& value);
	void clearData();
	void setBuilder(std::shared_ptr<SSDCommandBuilder> builder);
	NandData& getStorage();
	void clearBufferAndDirectory();
	void clearBuffer();

private:
	NandData& storage;
	CommandBuffer& cmdBuf;
	std::shared_ptr<SSDCommandBuilder> builder;
	string outputPath;

	static const int nandFileSize = 1200;
};
