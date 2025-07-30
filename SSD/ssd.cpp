#include "ssd.h"

#include <algorithm>

void 
SSD::setParser(SSDCommandParser* parser)
{
	this->parser = parser;
}

void
SSD::run(const string& commandStr)
{
	// parse command
	parser->setCommand(commandStr);
	if (!parser->isValidCommand()) {
		// print ERROR to ssd_output.txt
		return;
	}
	parsedCommand = parser->getCommandVector();

	clearData();
	readNandFile();

	// run command
	string operation = parsedCommand.at(1);
	int lba = std::stoi(parsedCommand.at(2));

	if (operation == "R") {
		string result = runReadCommand(lba);
		writeOutputFile(result);
	}
	else if (operation == "W") {
		runWriteCommand(lba, parsedCommand.at(3));
		writeNandFile();
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