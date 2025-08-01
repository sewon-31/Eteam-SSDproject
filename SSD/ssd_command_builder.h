#pragma once

#include "command.h"

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;

class SSDCommandBuilder
{
public:
	enum Index {
		OP = 0,
		LBA = 1,
		VAL = 2,
		SIZE = 2,
		MAX_ARG_LENGTH = 3
	};

	enum Size {
		MIN = 0,
		MAX = 10
	};

	virtual void setCommandVector(vector<string> inputCommandVector);
	virtual vector<string> getCommandVector() const;

	virtual bool isValidCommand() const;
	virtual std::shared_ptr<ICommand> createCommand(vector<string> commandVector);

	static constexpr const char* CMD_READ = "R";
	static constexpr const char* CMD_WRITE = "W";
	static constexpr const char* CMD_ERASE = "E";

private:
	bool isValidValue(const string& valueStr) const;
	bool isValidLBA(int lba, int size = 0) const;
	bool isValidSize(int size) const;

	vector<string> commandVector;
};