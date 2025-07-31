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
			return false;
		}

		// check operation command
		string opCommand = commandVector.at(OP);
		if (opCommand != CMD_READ && opCommand != CMD_WRITE) {
			return false;
		}

		// check parameter count for each operation case
		if (opCommand == CMD_READ && commandVector.size() != MAX_ARG_LENGTH - 1) {
			return false;
		}

		if (opCommand == CMD_WRITE && commandVector.size() != MAX_ARG_LENGTH) {
			return false;
		}

		// check lba range
		string lbaStr = commandVector.at(LBA);
		if (isValidLBA(std::stoi(lbaStr)) == false) {
			return false;
		}

		// check value
		if (opCommand == CMD_WRITE) {
			if (isValidValue(commandVector.at(VAL)) == false) {
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
	return lba >= NandData::LBA::MIN && lba <= NandData::LBA::MAX;
}