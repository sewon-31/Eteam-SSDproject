#pragma once

#include "command.h"

#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;

class SSDCommandParser
{
public:
	enum Index {
		OP = 0,
		LBA,
		VAL,
		MAX_ARG_LENGTH	// 3
	};

	virtual void setCommandVector(vector<string> inputCommandVector);
	virtual vector<string> getCommandVector() const;

	virtual bool isValidCommand() const;
	virtual std::shared_ptr<ICommand> createCommand(vector<string> commandVector, NandData& storage);

	static constexpr const char* CMD_READ = "R";
	static constexpr const char* CMD_WRITE = "W";

private:
	bool isValidValue(const string& valueStr) const;
	bool isValidLBA(int lba) const;

	vector<string> commandVector;
};