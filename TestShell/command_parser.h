#pragma once

#include <string>
#include <vector>

using std::string;
using std::vector;

class CommandParser
{
public:
	void setCommand(const string& command);
	const std::vector<std::string>& getCommandVector() const;
	bool isValidCommand() const;

	static constexpr const char* CMD_READ = "read";
	static constexpr const char* CMD_WRITE = "write";
	static constexpr const char* CMD_EXIT = "exit";
	static constexpr const char* CMD_HELP = "help";
	static constexpr const char* CMD_FULLREAD = "fullread";
	static constexpr const char* CMD_FULLWRITE = "fullwrite";
	static constexpr const char* CMD_ERASE = "erase";
	static constexpr const char* CMD_FLUSH = "flush";
	static constexpr const char* CMD_SCRIPT1 = "1_";
	static constexpr const char* CMD_SCRIPT1_NAME = "1_FullWriteAndReadCompare";
	static constexpr const char* CMD_SCRIPT2 = "2_";
	static constexpr const char* CMD_SCRIPT2_NAME = "2_PartialLBAWrite";
	static constexpr const char* CMD_SCRIPT3 = "3_";
	static constexpr const char* CMD_SCRIPT3_NAME = "3_WriteReadAging";

private:
	bool isValidValue(const string& valueStr) const;
	bool isValidSize(const string& sizeStr) const;

	string commandStr;
	vector<string> commandVector;
};