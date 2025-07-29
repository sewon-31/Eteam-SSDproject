#include "SSDCommandParser.h"

#include <sstream>

using std::istringstream;

void
SSDCommandParser::setCommand(const string& command)
{
	this->commandStr = command;
}

bool
SSDCommandParser::validateCommand()
{
	commandVector.clear();

	istringstream ss{ commandStr };
	string strBuf;

	while (getline(ss, strBuf, ' ')) {
		commandVector.push_back(strBuf);
	}
	
	if (commandVector.at(0) != "ssd") {
		return false;
	}

	return true;
}