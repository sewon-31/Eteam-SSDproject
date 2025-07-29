#include "SSDCommandParser.h"

#include <sstream>

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
SSDCommandParser::validateCommand()
{
	if (commandVector.at(0) != "ssd") {
		return false;
	}

	return true;
}