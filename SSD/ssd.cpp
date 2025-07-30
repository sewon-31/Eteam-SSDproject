#include "ssd.h"
#include "file_interface.h"
#include <algorithm>
#include <iostream>

void
SSD::setParser(SSDCommandParser* parser)
{
	this->parser = parser;
}

void
SSD::run(const string& commandStr)
{
	if (parser == nullptr) {
		SSDCommandParser myParser;
		parser = &myParser;
	}

	// parse command
	parser->setCommand(commandStr);
	if (!parser->isValidCommand()) {
		//writeOutputFile("ERROR");
		return;
	}
	parsedCommand = parser->getCommandVector();

	clearData();
	//readNandFile();

	// run command
	string operation = parsedCommand.at(SSDCommandParser::Index::OP);
	int lba = std::stoi(parsedCommand.at(SSDCommandParser::Index::LBA));

	if (operation == "R") {
		//std::cout << "Read" << lba << std::endl;
		string result = runReadCommand(lba);
		//writeOutputFile(result);
	}
	else if (operation == "W") {
		//std::cout << "Write" << lba << std::endl;
		runWriteCommand(lba, parsedCommand.at(SSDCommandParser::Index::VAL));
		//writeNandFile();
	}
}

string
SSD::runReadCommand(int lba)
{
	return data[lba];
}

void
SSD::clearData()
{
	std::fill(std::begin(data), std::end(data), "0x00000000");
}

void
SSD::runWriteCommand(int lba, const string& value)
{
	data[lba] = value;
}

string
SSD::getData(int lba) const
{
	return data[lba];
}

bool 
SSD::readNandFile() {
	bool ret;

	if (nandFile.checkSize() != 1200)  return false;

	for (int i = 0; i < 100; i++)
	{
		ret = nandFile.fileReadOneline(data[i]);

		if (!ret)  break;
	}
	nandFile.fileClose();
	return ret;
}

bool 
SSD::writeNandFile() {

	return true;
}

void 
SSD::writeOutputFile(const string& str) {

}