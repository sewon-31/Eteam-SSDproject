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

const std::vector<std::string>& CommandParser::getCommandVector() const
{
	return commandVector;
}

bool CommandParser::isValidOpCommand() const
{
	if (commandVector.size() < 1) return false;

	// check operation command
	string opCommand = commandVector.at(0);

	// check valid commands
	if (opCommand != CMD_READ
		&& opCommand != CMD_WRITE
		&& opCommand != CMD_EXIT
		&& opCommand != CMD_HELP
		&& opCommand != CMD_FULLREAD
		&& opCommand != CMD_FULLWRITE
		&& opCommand != CMD_SCRIPT1
		&& opCommand != CMD_SCRIPT1_NAME
		&& opCommand != CMD_SCRIPT2
		&& opCommand != CMD_SCRIPT2_NAME
		&& opCommand != CMD_SCRIPT3
		&& opCommand != CMD_SCRIPT3_NAME) {
		return false;
	}
}

bool CommandParser::isValidCommandParameter() const
{
	try {
		// check parameter count
		if (commandVector.size() < 1 || commandVector.size() > 3) {
			return false;
		}

		string opCommand = commandVector.at(0);
		// check parameter count for each operation case
		if ((opCommand == CMD_READ && commandVector.size() != 2) ||
			(opCommand == CMD_WRITE && commandVector.size() != 3) ||
			(opCommand == CMD_EXIT && commandVector.size() != 1) ||
			(opCommand == CMD_HELP && commandVector.size() != 1) ||
			(opCommand == CMD_FULLREAD && commandVector.size() != 1) ||
			(opCommand == CMD_FULLWRITE && commandVector.size() != 2) ||
			((opCommand == CMD_SCRIPT1 || opCommand == CMD_SCRIPT1_NAME) && commandVector.size() != 1) ||
			((opCommand == CMD_SCRIPT2 || opCommand == CMD_SCRIPT2_NAME) && commandVector.size() != 1) ||
			((opCommand == CMD_SCRIPT3 || opCommand == CMD_SCRIPT3_NAME) && commandVector.size() != 1)) {
			return false;
		}

		// check lba range
		if (opCommand == CMD_READ || opCommand == CMD_WRITE) {
			string lbaStr = commandVector.at(1);
			int lba = std::stoi(lbaStr);
			if (lba < 0 || lba > 99) {
				return false;
			}
		}

		// check value format
		if (opCommand == CMD_WRITE) {
			if (!isValidValue(commandVector.at(2))) {
				return false;
			}
		}

		if (opCommand == CMD_FULLWRITE) {
			if (!isValidValue(commandVector.at(1))) {
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