#include "SSDCommandParser.h"

#include <sstream>
#include <iostream>

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
		commandVector.push_back(strBuf);
	}
}

bool
SSDCommandParser::isValidCommand() const
{
	try {
		// check parameter count
		if (commandVector.size() < 3 || commandVector.size() > 4) {
			return false;
		}

		// check app name
		if (commandVector.at(0) != APP_NAME) {
			return false;
		}

		// check operation command
		string opCommand = commandVector.at(1);
		if (opCommand != CMD_READ && opCommand != CMD_WRITE) {
			return false;
		}

		// check parameter count for each operation case
		if (opCommand == CMD_READ && commandVector.size() != 3) {
			return false;
		}

		if (opCommand == CMD_WRITE && commandVector.size() != 4) {
			return false;
		}

		// check lba range
		string lbaStr = commandVector.at(2);
		int lba = std::stoi(lbaStr);
		if (lba < 0 || lba > 99) {
			return false;
		}

		// check value
		if (opCommand == CMD_WRITE) {
			if (isValidValue(commandVector.at(3)) == false) {
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
	if (valueStr.length() != 10) {
		return false;
	}

	// try to parse value into hex number
	try {
		size_t pos = 0;
		unsigned long value = std::stoul(valueStr, &pos, 0);

		if (pos != valueStr.length()) {
			return false;
		}

		if (value > 0xFFFFFFFF) {
			return false;
		}

		uint32_t out = static_cast<uint32_t>(value);
		return true;
	}
	catch (...) {
		return false;
	}
}
