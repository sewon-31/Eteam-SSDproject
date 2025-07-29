#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

class SSDCommandParser
{
public:
	void setCommand(const string& command);
	bool validateCommand();

private:
	bool validateValue(const string& valueStr);

	string commandStr;
	vector<string> commandVector;

	const string CMD_READ = "R";
	const string CMD_WRITE = "W";
};