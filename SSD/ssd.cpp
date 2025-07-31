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

	clearData();
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
		ret = nandFile.fileReadOneline(data[i]);

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
		ret = nandFile.fileWriteOneline(data[i]);

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

bool
SSD::reduceCMDBuffer(TEST_CMD in, TEST_CMD out) {
	bool ret = true;

	int virtual_op[100];	// 9 == NULL, 7 = E, 0-5 = W 

	const int OP_NULL = 9;
	const int OP_E = 7;
	const int OP_W_MAX = 5;

	// 1. Replcae w iba "0x00000000" >  E iba
	// 2. make virtual data 
	// 3. Make CMD
		//3-1. Make new CMD E range check and W  
		//3-2. single W (n+1 , n-1 is E)  2 E , 1 W-> merge E, 1W && merge E len <= 10

	// display
	for (int idx_cb = 0; idx_cb < 6; idx_cb++) {
		std::cout << in.op[idx_cb] << " " << in.lba[idx_cb] << " " << in.data[idx_cb] << " " << in.size[idx_cb] << "\n";
	}

	// step - 1
	for (int idx_cb = 0 ; idx_cb < 6; idx_cb++) {
		if (in.op[idx_cb] == "W" && in.data[idx_cb] == "0x00000000") {
			in.op[idx_cb] = "E";
			in.size[idx_cb] = 1;
		}
	}

	// step - 2
	for (int idx_iba = 0; idx_iba < 100; idx_iba++) {
		virtual_op[idx_iba] = OP_NULL;
	}

	for (int idx_cb = 0; idx_cb < 6; idx_cb++) {
		if (in.op[idx_cb] == "W") {
			virtual_op[in.lba[idx_cb]] = idx_cb;
		}
		else if (in.op[idx_cb] == "E") {
			for (int idx_size = 0; idx_size < in.size[idx_cb]; idx_size++)
				virtual_op[in.lba[idx_cb] + idx_size] = OP_E;
		}
	}
	// display
	for (int idx_iba = 0; idx_iba < 100; idx_iba++) {
		if (virtual_op[idx_iba] == OP_NULL)
			std::cout << "N" << " ";
		else if (virtual_op[idx_iba] == OP_E)
			std::cout << "E" << " ";
		else 
			std::cout << "W" << " ";


		if (idx_iba % 10 == 9)
			std::cout  << "\n";
	}

	// step - 3
	int continue_E_CMD = 0;
	int newCMDCount = 0;

	for (int idx_iba = 0; idx_iba < 100; idx_iba++) {

		// Check E
		if (virtual_op[idx_iba] == OP_E) {
			if (continue_E_CMD == 0) {
				out.op[newCMDCount] = "E";
				out.lba[newCMDCount] = idx_iba;
				out.size[newCMDCount] = 1;
				continue_E_CMD = 1;
			}
			else {
				out.size[newCMDCount]++;
				continue_E_CMD++;
			}
			continue;
		}
		else if (continue_E_CMD > 0) {
			continue_E_CMD = 0;
			newCMDCount++;
		}

		//Check W
		if (virtual_op[idx_iba] <= OP_W_MAX) {
			out.op[newCMDCount] = "W";
			out.lba[newCMDCount] = idx_iba;
			out.data[newCMDCount] = in.data[virtual_op[idx_iba]];
			out.size[newCMDCount] = 1;
			newCMDCount++;
		}
	}
	// display
	for (int idx_cb = 0; idx_cb < newCMDCount; idx_cb++) {
		std::cout << out.op[idx_cb] << " " << out.lba[idx_cb] << " " << out.data[idx_cb] << " " << out.size[idx_cb] << "\n";
	}

	return ret;
}