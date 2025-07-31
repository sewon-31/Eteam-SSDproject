#pragma once

#include <string>
#include <vector>

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

	virtual void setCommandVector(vector<string> commandVector);
	virtual bool isValidCommand() const;
	virtual vector<string> getCommandVector() const;

private:
	bool isValidValue(const string& valueStr) const;

	string commandStr;
	vector<string> commandVector;
public:
};