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

	return true;
}