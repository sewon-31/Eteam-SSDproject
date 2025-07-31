#include "ssd_command_builder.h"

#include <sstream>
#include <iostream>
#include <regex>

using std::istringstream;

void
SSDCommandBuilder::setCommandVector(vector<string> inputCommandVector)
{
	commandVector = inputCommandVector;
}

vector<string>
SSDCommandBuilder::getCommandVector() const
{
	return commandVector;
}

bool
SSDCommandBuilder::isValidCommand() const
{
	try {
		// check parameter count
		if (commandVector.size() < MAX_ARG_LENGTH - 1
			|| commandVector.size() > MAX_ARG_LENGTH) {
			std::cout << "1?\n";
			return false;
		}

		// check operation command
		string opCommand = commandVector.at(OP);
		if (opCommand != CMD_READ && opCommand != CMD_WRITE && opCommand != CMD_ERASE) {
			std::cout << "2?\n";
			return false;
		}

		// check parameter count for each operation case
		if (opCommand == CMD_READ && commandVector.size() != MAX_ARG_LENGTH - 1) {
			std::cout << "3?\n";
			return false;
		}

		if ((opCommand == CMD_WRITE || opCommand == CMD_ERASE)
			&& commandVector.size() != MAX_ARG_LENGTH) {
			std::cout << "4?\n";
			return false;
		}

		// check lba range
		int lba = std::stoi(commandVector.at(LBA));
		if (isValidLBA(lba) == false) {
			std::cout << "5?\n";
			return false;
		}

		// check value (write)
		if (opCommand == CMD_WRITE) {
			if (isValidValue(commandVector.at(VAL)) == false) {
				std::cout << "6?\n";
				return false;
			}
		}

		// chek size (erase)
		if (opCommand == CMD_ERASE) {

			// check size validity
			int size = std::stoi(commandVector.at(SIZE));

			if (isValidSize(size) == false) {
				std::cout << "7?\n";
				return false;
			}

			// check lba range
			if (isValidLBA(lba, size) == false) {
				std::cout << "8?\n";
				return false;
			}
		}

		return true;
	}
	catch (...) {
		return false;
	}
}

std::shared_ptr<ICommand>
SSDCommandBuilder::createCommand(vector<string> inputCommandVector, NandData& storage)
{
	setCommandVector(inputCommandVector);

	if (!isValidCommand()) {
		return nullptr;
	}
	
	// convert lba into int
	int lba = std::stoi(commandVector.at(LBA));

	// create command
	string opCommand = commandVector.at(OP);
	if (opCommand == CMD_READ) {
		return std::make_shared<ReadCommand>(storage, lba);
	}
	else if (opCommand == CMD_WRITE) {
		return std::make_shared<WriteCommand>(storage, lba, commandVector.at(VAL));
	}
	else if (opCommand == CMD_ERASE) {
		int end = lba + std::stoi(commandVector.at(SIZE)) - 1;
		return std::make_shared<EraseCommand>(storage, lba, end);
	}
	else {
		return nullptr;
	}
}

bool
SSDCommandBuilder::isValidValue(const string& valueStr) const
{
	std::regex re("^0x[0-9A-F]{8}$");
	return std::regex_match(valueStr, re);
}

bool
SSDCommandBuilder::isValidLBA(int lba) const
{
	if (size == 0) {
		return lba >= NandData::LBA::MIN && lba <= NandData::LBA::MAX;
	}

	return lba + size <= NandData::LBA::MAX;
}

bool
SSDCommandParser::isValidSize(int size) const
{
	return size >= Size::MIN && size <= Size::MAX;
}