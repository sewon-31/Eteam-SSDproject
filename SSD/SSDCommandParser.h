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
	string commandStr;
	vector<string> commandVector;
};