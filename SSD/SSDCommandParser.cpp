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

	//for (auto x : commandVector)
	//{
	//	std::cout << x << std::endl;
	//}
}

bool
SSDCommandParser::validateCommand()
{
	// check parameter count
	if (commandVector.size() < 3 || commandVector.size() > 4) {
		return false;
	}

	// check app name
	if (commandVector.at(0) != "ssd") {
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
	try {
		int lba = std::stoi(lbaStr);
		if (lba < 0 || lba > 99) {
			return false;
		}
	}
	catch (const std::exception& e) {
		return false;
	}

	return true;
}