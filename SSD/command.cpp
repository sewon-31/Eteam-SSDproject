#include "command.h"

// BaseCommand
BaseCommand::BaseCommand(NandData& storage, int lba)
	: storage(storage), lba(lba)
{
}

// ReadCommand
void
ReadCommand::run(string& result)
{
	storage.clear();

	// updateNandDataFromBuffer();
	storage.updateFromFile();

	execute(result);
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

// WriteCommand
WriteCommand::WriteCommand(NandData& storage, int lba, const std::string& value)
	: BaseCommand(storage, lba), value(value)
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

// EraseCommand
EraseCommand::EraseCommand(NandData& storage, int lba, int size)
	: BaseCommand(storage, lba), size(size)
{
}

void
EraseCommand::run(string& result)
{
	// storage.erase();
}

void
EraseCommand::execute(string& result)
{
	// storage.erase();
}

CmdType
EraseCommand::getCmdType() const
{
	return CmdType::ERASE;
}