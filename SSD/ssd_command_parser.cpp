#include "ssd_command_parser.h"

#include <sstream>
#include <iostream>
#include <regex>

using std::istringstream;

void
SSDCommandParser::setCommand(const string& command)
{
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

		string CMD_READ = "R";
		string CMD_WRITE = "W";

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

bool
SSDCommandParser::isValidValue(const string& valueStr) const
{
	std::regex re("^0x[0-9A-F]{8}$");
	return std::regex_match(valueStr, re);
}
