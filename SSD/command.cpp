#include "command.h"

// BaseCommand
BaseCommand::BaseCommand(int lba)
	: storage(NandData::getInstance()), lba(lba)
{
}

int
BaseCommand::getLBA() const
{
	return lba;
}

// ReadCommand
void
ReadCommand::run(string& result)
{
	storage.clear();

	string readData = updateNandDataFromBuffer();
	if (readData == "") {
		storage.updateFromFile();
		execute(result);
	}
	
	std::cout << result << std::endl;
}

void
ReadCommand::execute(string& result)
{
	result = storage.read(lba);
}

CmdType
ReadCommand::getCmdType() const
{
	return CmdType::READ;
}

string ReadCommand::updateNandDataFromBuffer()
{
	std::vector<std::shared_ptr<ICommand>> buffers = CommandBuffer::getInstance().getBuffer();

	string result = "";
	for (auto command : buffers) {
		if (command->getCmdType() == CmdType::ERASE) result = "0x00000000";
	}
	return result;
}

// WriteCommand
WriteCommand::WriteCommand(int lba, const std::string& value)
	: BaseCommand(lba), value(value)
{
}

void
WriteCommand::run(string& result)
{
	storage.clear();
	storage.updateFromFile();

	execute(result);

	storage.updateToFile();
}

void
WriteCommand::execute(string& result)
{
	storage.write(lba, value);
}

CmdType
WriteCommand::getCmdType() const
{
	return CmdType::WRITE;
}

string
WriteCommand::getValue() const
{
	return value;
} 

// EraseCommand
EraseCommand::EraseCommand(int lba, int size)
	: BaseCommand(lba), size(size)
{
}

void
EraseCommand::run(string& result)
{
	storage.clear();
	storage.updateFromFile();

	execute(result);

	storage.updateToFile();
}

void
EraseCommand::execute(string& result)
{
	storage.erase(lba, lba + size - 1);
}

CmdType
EraseCommand::getCmdType() const
{
	return CmdType::ERASE;
}

int
EraseCommand::getSize() const
{
	return size;
}