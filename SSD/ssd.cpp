#include "ssd.h"
#include "file_interface.h"
#include <algorithm>
#include <iostream>

SSD::SSD(const std::string& nandPath, const std::string& outputPath)
	: nandFile(nandPath), outputFile(outputPath) {
}

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
		writeOutputFile("ERROR");
		return;
	}
	parsedCommand = parser->getCommandVector();

	storage.clear();
	readNandFile();

	// run command
	string operation = parsedCommand.at(SSDCommandParser::Index::OP);
	int lba = std::stoi(parsedCommand.at(SSDCommandParser::Index::LBA));

	if (operation == "R") {
		//std::cout << "Read" << lba << std::endl;
		string result = runReadCommand(lba);
		writeOutputFile(result);
	}
	else if (operation == "W") {
		//std::cout << "Write" << lba << std::endl;
		runWriteCommand(lba, parsedCommand.at(SSDCommandParser::Index::VAL));
		writeNandFile();
	}
}

string
SSD::runReadCommand(int lba)
{
	return storage.read(lba);
}

void
SSD::runWriteCommand(int lba, const string& value)
{
	storage.write(lba, value);
}

string
SSD::getData(int lba) const
{
	return storage.read(lba);
}

void
SSD::clearData()
{
	storage.clear();
}

FileInterface& 
SSD::getNandFile() {
	return nandFile;
}

FileInterface&
SSD::getOutputFile() {
	return outputFile;
}

bool 
SSD::readNandFile() {
	bool ret;
	
	nandFile.fileOpen();

	if (nandFile.checkSize() != nandFileSize)  return false;

	for (int i = 0; i < maxLbaNum; i++)
	{
		string data;
		ret = nandFile.fileReadOneline(data);
		storage.write(i, data);

		if (!ret)  break;
	}
	nandFile.fileClose();
	return ret;
}

bool 
SSD::writeNandFile() {
	bool ret;

	nandFile.fileClear();
	nandFile.fileOpen();

	for (int i = 0; i < maxLbaNum; i++)
	{
		ret = nandFile.fileWriteOneline(storage.read(i));

		if (!ret)
			break;
	}
	nandFile.fileClose();
	return ret;
}

bool 
SSD::writeOutputFile(const string& str) {
	bool ret;

	outputFile.fileClear();
	outputFile.fileOpen();
	ret = outputFile.fileWriteOneline(str);
	outputFile.fileClose();
	return ret;
}