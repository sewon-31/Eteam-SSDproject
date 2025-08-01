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

	result = fastReadFromBuffer();

	if (result == INVALID) {
		storage.updateFromFile();
		execute(result);
	}
#if _DEBUG
	std::cout << result << std::endl; // for debug
#endif
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

string ReadCommand::fastReadFromBuffer()
{
	std::vector<std::shared_ptr<ICommand>> buffers = CommandBuffer::getInstance().getBuffer();

	string result = INVALID;
	for (auto command : buffers) {
		
		if (command->getCmdType() == CmdType::ERASE) {
			std::shared_ptr<EraseCommand> erase = std::dynamic_pointer_cast<EraseCommand>(command);

			int startLBA = erase->getLBA();
			int endLBA = startLBA + erase->getSize() - 1;

			if (lba >= startLBA && lba <= endLBA) result = ERASE_DATA;
		}
		if (command->getCmdType() == CmdType::WRITE) {
			std::shared_ptr<WriteCommand> write = std::dynamic_pointer_cast<WriteCommand>(command);
			if (write->getLBA() == lba) result = write->getValue();
		}
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

// FlushCommand
void
FlushCommand::run(string& result)
{
	CommandBuffer::getInstance().flushBuffer();
}

void
FlushCommand::execute(string& result)
{
}

CmdType
FlushCommand::getCmdType() const
{
	return CmdType::FLUSH;
}