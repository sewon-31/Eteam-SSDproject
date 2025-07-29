#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

class SSDCommandParser
{
public:
	void setCommand(const string& command);
	bool isValidCommand() const;

private:
	bool isValidValue(const string& valueStr) const;

	string commandStr;
	vector<string> commandVector;

	const string CMD_READ = "R";
	const string CMD_WRITE = "W";
	const string APP_NAME = "ssd";
};