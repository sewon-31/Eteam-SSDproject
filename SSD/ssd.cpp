#include "ssd.h"
#include "file_interface.h"
#include "command_buffer.h"

#include <algorithm>
#include <iostream>
#include <sstream>

SSD::SSD(const std::string& nandPath, const std::string& outputPath)
	: outputPath(outputPath),
	storage(NandData::getInstance(nandPath)),
	cmdBuf(CommandBuffer::getInstance())
{
}

void
SSD::run(vector<string> commandVector)
{
	if (!builder) {
		builder = std::make_shared<SSDCommandBuilder>();
	}

	string result("");

	// create command (validity check included)
	auto cmd = builder->createCommand(commandVector);
	if (cmd == nullptr) {
		updateOutputFile("ERROR");
		return;
	}

	cmdBuf.Init();

	auto type = cmd->getCmdType();
	if (type == CmdType::READ || type == CmdType::FLUSH)
		cmd->run(result);
	else if (type == CmdType::WRITE || type == CmdType::ERASE)
		cmdBuf.addCommand(cmd);

	cmdBuf.updateToDirectory();

	if (!result.empty()) {
		updateOutputFile(result);
	}
}

bool
SSD::updateOutputFile(const string& result)
{
	FileInterface::clearFile(outputPath);
	return FileInterface::writeLine(outputPath, result, /*append=*/false);
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

void
SSD::clearBufferAndDirectory()
{
	cmdBuf.clearBuffer();
	cmdBuf.updateToDirectory();
}

void
SSD::clearBuffer()
{
	cmdBuf.clearBuffer();
}
