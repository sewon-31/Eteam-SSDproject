#include "command_parser.h"
#include "command.h"

#include <sstream>
#include <iostream>
#include <regex>

using std::istringstream;

void CommandParser::setCommand(const string& command)
{
	this->commandStr = command;
	commandVector.clear();

	istringstream ss{ commandStr };
	string strBuf;

	while (getline(ss, strBuf, ' ')) {
		if (!strBuf.empty()) {
			commandVector.push_back(strBuf);
		}
	}
}

const std::vector<std::string>& CommandParser::getCommandVector() const
{
	return commandVector;
}

bool CommandParser::isValidCommand() const
{
	try {
		// check parameter count
		if (commandVector.size() < 1 || commandVector.size() > 3) {
			return false;
		}

		// check operation command
		string opCommand = commandVector.at(INPUT_IDX_OPCOMMAND);

		// check valid commands
		if (opCommand != CMD_READ
			&& opCommand != CMD_WRITE
			&& opCommand != CMD_EXIT
			&& opCommand != CMD_HELP
			&& opCommand != CMD_FULLREAD
			&& opCommand != CMD_FULLWRITE
			&& opCommand != CMD_ERASE
			&& opCommand != CMD_ERASE_RANGE
			&& opCommand != CMD_FLUSH
			&& opCommand != CMD_SCRIPT1
			&& opCommand != CMD_SCRIPT1_NAME
			&& opCommand != CMD_SCRIPT2
			&& opCommand != CMD_SCRIPT2_NAME
			&& opCommand != CMD_SCRIPT3
			&& opCommand != CMD_SCRIPT3_NAME
			&& opCommand != CMD_SCRIPT4
			&& opCommand != CMD_SCRIPT4_NAME) {
			return false;
		}

		// check parameter count for each operation case
		if ((opCommand == CMD_READ && commandVector.size() != NUM_INPUTS_FOR_READ) ||
			(opCommand == CMD_WRITE && commandVector.size() != NUM_INPUTS_FOR_WRITE) ||
			(opCommand == CMD_EXIT && commandVector.size() != NUM_INPUTS_FOR_EXIT) ||
			(opCommand == CMD_HELP && commandVector.size() != NUM_INPUTS_FOR_HELP) ||
			(opCommand == CMD_FULLREAD && commandVector.size() != NUM_INPUTS_FOR_FULLREAD) ||
			(opCommand == CMD_FULLWRITE && commandVector.size() != NUM_INPUTS_FOR_FULLWRITE) ||
			(opCommand == CMD_ERASE && commandVector.size() != NUM_INPUTS_FOR_ERASE) ||
			(opCommand == CMD_ERASE_RANGE && commandVector.size() != NUM_INPUTS_FOR_ERASE_RANGE) ||
			(opCommand == CMD_FLUSH && commandVector.size() != NUM_INPUTS_FOR_FLUSH) ||
			((opCommand == CMD_SCRIPT2 || opCommand == CMD_SCRIPT2_NAME) && commandVector.size() != NUM_INPUTS_FOR_TESTSCRIPT) ||
			((opCommand == CMD_SCRIPT3 || opCommand == CMD_SCRIPT3_NAME) && commandVector.size() != NUM_INPUTS_FOR_TESTSCRIPT) ||
			((opCommand == CMD_SCRIPT1 || opCommand == CMD_SCRIPT1_NAME) && commandVector.size() != NUM_INPUTS_FOR_TESTSCRIPT) ||
			((opCommand == CMD_SCRIPT4 || opCommand == CMD_SCRIPT4_NAME) && commandVector.size() != NUM_INPUTS_FOR_TESTSCRIPT)) {
			return false;
		}

		// check lba range
		if (opCommand == CMD_READ || opCommand == CMD_WRITE || opCommand == CMD_ERASE) {
			int lba = std::stoi(commandVector.at(INPUT_IDX_LBA));
			if (!isValidLBA(lba)) return false;
		}

		if (opCommand == CMD_ERASE_RANGE) {
			int lba1 = std::stoi(commandVector.at(INPUT_IDX_START_LBA));
			int lba2 = std::stoi(commandVector.at(INPUT_IDX_END_LBA));

			if (!isValidLBA(lba1)) return false;
			if (!isValidLBA(lba2)) return false;
		}

		// check value format
		if (opCommand == CMD_WRITE) {
			if (!isValidValue(commandVector.at(INPUT_IDX_WRITE_VALUE))) {
				return false;
			}
		}

		if (opCommand == CMD_FULLWRITE) {
			if (!isValidValue(commandVector.at(INPUT_IDX_FULL_WRITE_VALUE))) {
				return false;
			}
		}

		if (opCommand == CMD_ERASE) {
			if (!isValidSize(commandVector.at(INPUT_IDX_ERASE_SIZE))) {
				return false;
			}
		}

		return true;
	}
	catch (...) {
		return false;
	}
}

bool CommandParser::isValidLBA(int lba) const
{
	if (lba < Command::MIN_LBA || lba > Command::MAX_LBA) return false;
	return true;
}

bool CommandParser::isValidValue(const string& valueStr) const
{
	std::regex re("^0x[0-9A-F]{8}$");
	return std::regex_match(valueStr, re);
}

bool CommandParser::isValidSize(const string& sizeStr) const
{
	try {
		int size = std::stoi(sizeStr);
	}
	catch (...) {
		return false;
	}
	return true;
}
