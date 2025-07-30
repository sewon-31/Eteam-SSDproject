#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

class CommandParser
{
public:
	void setCommand(const string& command);
	bool isValidCommand() const;

private:
	bool isValidValue(const string& valueStr) const;

	string commandStr;
	vector<string> commandVector;
	
	const string CMD_READ = "read";
	const string CMD_WRITE = "write";
	const string CMD_EXIT = "exit";
	const string CMD_HELP = "help";
	const string CMD_FULLREAD = "fullread";
	const string CMD_FULLWRITE = "fullwrite";
};