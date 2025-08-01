#include "ssd.h"
#include "file_interface.h"
#include "command_buffer.h"

#include <algorithm>
#include <iostream>

SSD::SSD(const std::string& nandPath, const std::string& outputPath)
	: outputFile(outputPath),
	storage(NandData::getInstance()),
	cmdBuf(CommandBuffer::getInstance())
{
}

void
SSD::run(vector<string> commandVector)
{
	if (!builder) {
		builder = std::make_shared<SSDCommandBuilder>();
	}

	cmdBuf.Init();
	string result("");

	// create command (validity check included)
	auto cmd = builder->createCommand(commandVector);
	if (cmd == nullptr) {
		result = "ERROR";
	}
	else {
		auto type = cmd->getCmdType();
		//if (type == CmdType::READ || type == CmdType::FLUSH
		//{
		cmd->run(result);
		//}
		//else if(type==CmdType::WRITE || type ==CmdType::ERASE {
			//cmdBuf.addCommand(cmd);
		//}
	}

	cmdBuf.updateToDirectory();
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
SSD::setBuilder(std::shared_ptr<SSDCommandBuilder> builder)
{
	this->builder = builder;
}

NandData&
SSD::getStorage()
{
	return storage;
}