#include "ssd.h"
#include "file_interface.h"
#include <algorithm>
#include <iostream>

SSD::SSD(const std::string& nandPath, const std::string& outputPath)
	: outputFile(outputPath)
{
	// create storage
	storage = NandData( nandPath );
}

void
SSD::run(vector<string> commandVector)
{
	if (!parser) {
		parser = std::make_shared<SSDCommandParser>();
	}

	// parse command
	auto cmd = parser->createCommand(commandVector, storage);
	if (cmd == nullptr) {
		updateOutputFile("ERROR");
		return;
	}

	string result("");

	//if (cmd->getCmdType() == CmdType::READ) {
	cmd->run(result);
	//}
	//else {
		// add to buffer
	//}

	if (!result.empty()) {
		updateOutputFile(result);
	}
}

bool 
SSD::updateOutputFile(const string& result)
{
	outputFile.fileClear();
	outputFile.fileOpen();

	bool ret = outputFile.fileWriteOneline(result);

	outputFile.fileClose();
	return ret;
}

string
SSD::getData(int lba) const
{
	return storage.read(lba);
}

void
SSD::writeData(int lba, const string& value)
{
	storage.write(lba, value);
}

void
SSD::clearData()
{
	storage.clear();
}

void
SSD::setParser(std::shared_ptr<SSDCommandParser> parser)
{
	this->parser = parser;
}

NandData& 
SSD::getStorage()
{
	return storage;
}

FileInterface&
SSD::getOutputFile() 
{
	return outputFile;
}