#include "ssd_command_parser.h"

#include <sstream>
#include <iostream>
#include <regex>

using std::istringstream;

void
SSDCommandParser::setCommandVector(vector<string> inputCommandVector)
{
	/*
	// set commandStr
	this->commandStr = command;

	// set commandVector
	commandVector.clear();

	istringstream ss{ commandStr };
	string strBuf;

	while (getline(ss, strBuf, ' ')) {
		if (!strBuf.empty()) {
			commandVector.push_back(strBuf);
		}
	}
	*/

	commandVector = inputCommandVector;
}

vector<string>
SSDCommandParser::getCommandVector() const
{
	return commandVector;
}

bool
SSDCommandParser::isValidCommand() const
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
		int lba = std::stoi(lbaStr);
		if (lba < 0 || lba > 99) {
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
SSDCommandParser::createCommand(vector<string> inputCommandVector, NandData& storage)
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
SSDCommandParser::isValidValue(const string& valueStr) const
{
	std::regex re("^0x[0-9A-F]{8}$");
	return std::regex_match(valueStr, re);
}
