#include "command_parser.h"

#include <sstream>
#include <iostream>
#include <regex>

using std::istringstream;

void CommandParser::setCommand(const string& command)
{
	this->commandStr = command;
	commandVector.clear();

	istringstream ss{ commandStr };
	string strBuf;

	while (getline(ss, strBuf, ' ')) {
		if (!strBuf.empty()) {
			commandVector.push_back(strBuf);
		}
	}
}

bool CommandParser::isValidCommand() const
{
	try {
		// check parameter count
		if (commandVector.size() < 1 || commandVector.size() > 3) {
			return false;
		}

		// check operation command
		string opCommand = commandVector.at(0);

		if (opCommand != CMD_READ
			&& opCommand != CMD_WRITE
			&& opCommand != CMD_EXIT
			&& opCommand != CMD_HELP
			&& opCommand != CMD_FULLREAD
			&& opCommand != CMD_FULLWRITE
			) {
			return false;
		}

		// check parameter count for each operation case
		if (opCommand == CMD_READ && commandVector.size() != 2) {
			return false;
		}

		if (opCommand == CMD_WRITE && commandVector.size() != 3) {
			return false;
		}

		if (opCommand == CMD_EXIT && commandVector.size() != 1) {
			return false;
		}

		if (opCommand == CMD_HELP && commandVector.size() != 1) {
			return false;
		}

		if (opCommand == CMD_FULLREAD && commandVector.size() != 1) {
			return false;
		}

		if (opCommand == CMD_FULLWRITE && commandVector.size() != 2) {
			return false;
		}

		if (opCommand == CMD_READ || opCommand == CMD_WRITE) {
			// check lba range
			string lbaStr = commandVector.at(1);
			int lba = std::stoi(lbaStr);
			if (lba < 0 || lba > 99) {
				return false;
			}
		}

		// check value
		if (opCommand == CMD_WRITE) {
			if (isValidValue(commandVector.at(2)) == false) {
				return false;
			}
		}

		if (opCommand == CMD_FULLWRITE) {
			if (isValidValue(commandVector.at(1)) == false) {
				return false;
			}
		}
		return true;
	}
	catch (...) {
		return false;
	}
}

bool CommandParser::isValidValue(const string& valueStr) const
{
	std::regex re("^0x[0-9A-F]{8}$");
	return std::regex_match(valueStr, re);
}