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
	static constexpr const char* CMD_ERASE_RANGE = "erase_range";
	static constexpr const char* CMD_FLUSH = "flush";
	static constexpr const char* CMD_SCRIPT1 = "1_";
	static constexpr const char* CMD_SCRIPT1_NAME = "1_FullWriteAndReadCompare";
	static constexpr const char* CMD_SCRIPT2 = "2_";
	static constexpr const char* CMD_SCRIPT2_NAME = "2_PartialLBAWrite";
	static constexpr const char* CMD_SCRIPT3 = "3_";
	static constexpr const char* CMD_SCRIPT3_NAME = "3_WriteReadAging";
	static constexpr const char* CMD_SCRIPT4 = "4_";
	static constexpr const char* CMD_SCRIPT4_NAME = "4_EraseAndWriteAging";

private:
	bool isValidValue(const string& valueStr) const;
	bool isValidSize(const string& sizeStr) const;
	bool isValidLBA(const int lba) const;

	string commandStr;
	vector<string> commandVector;

	const int NUM_INPUTS_FOR_READ = 2;
	const int NUM_INPUTS_FOR_WRITE = 3;
	const int NUM_INPUTS_FOR_EXIT = 1;
	const int NUM_INPUTS_FOR_HELP = 1;
	const int NUM_INPUTS_FOR_FULLREAD = 1;
	const int NUM_INPUTS_FOR_FULLWRITE = 2;
	const int NUM_INPUTS_FOR_ERASE = 3;
	const int NUM_INPUTS_FOR_ERASE_RANGE = 3;
	const int NUM_INPUTS_FOR_FLUSH = 1;
	const int NUM_INPUTS_FOR_TESTSCRIPT = 1;

	const int INPUT_IDX_OPCOMMAND = 0;
	const int INPUT_IDX_LBA = 1;
	const int INPUT_IDX_START_LBA = 1;
	const int INPUT_IDX_END_LBA = 2;
	const int INPUT_IDX_WRITE_VALUE = 2;
	const int INPUT_IDX_FULL_WRITE_VALUE = 1;
	const int INPUT_IDX_ERASE_SIZE = 2;
};