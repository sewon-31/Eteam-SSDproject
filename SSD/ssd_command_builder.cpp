#include "ssd_command_builder.h"

#include <sstream>
#include <regex>

using std::istringstream;

void
SSDCommandBuilder::setCommandVector(vector<string> inputCommandVector)
{
	commandVector = inputCommandVector;
}

vector<string>
SSDCommandBuilder::getCommandVector() const
{
	return commandVector;
}

bool
SSDCommandBuilder::isValidCommand() const
{
	try {
		// check parameter count
		if (commandVector.size() < MAX_ARG_LENGTH - 2
			|| commandVector.size() > MAX_ARG_LENGTH) {
			return false;
		}

		// check operation command
		string opCommand = commandVector.at(OP);
		if (!isValidOp(opCommand)) {
			return false;
		}

		// for flush, just check parameter count and return
		if (opCommand == CMD_FLUSH) {
			if (commandVector.size() != MAX_ARG_LENGTH - 2) {
				return false;
			}
			return true;
		}

		// check parameter count for each operation case
		if (opCommand == CMD_READ 
			&& commandVector.size() != MAX_ARG_LENGTH - 1) {
			return false;
		}

		if ((opCommand == CMD_WRITE || opCommand == CMD_ERASE)
			&& commandVector.size() != MAX_ARG_LENGTH) {
			return false;
		}

		// check lba range
		int lba = std::stoi(commandVector.at(LBA));
		if (!isValidLBA(lba)) {
			return false;
		}

		// check value (write)
		if (opCommand == CMD_WRITE) {
			if (!isValidValue(commandVector.at(VAL))) {
				return false;
			}
		}

		// chek size (erase)
		if (opCommand == CMD_ERASE) {

			// check size validity
			int size = std::stoi(commandVector.at(SIZE));

			if (!isValidSize(size)) {
				return false;
			}

			// check lba range
			if (!isValidLBA(lba, size)) {
				return false;
			}
		}

		return true;
	}
	catch (...) {
		return false;
	}
}

std::shared_ptr<ICommand>
SSDCommandBuilder::createCommand(vector<string> inputCommandVector)
{
	setCommandVector(inputCommandVector);

	if (!isValidCommand()) {
		return nullptr;
	}
	
	// convert lba into int
	int lba = std::stoi(commandVector.at(LBA));

	// create command
	string opCommand = commandVector.at(OP);
	if (opCommand == CMD_READ) {
		return std::make_shared<ReadCommand>(lba);
	}
	else if (opCommand == CMD_WRITE) {
		return std::make_shared<WriteCommand>(lba, commandVector.at(VAL));
	}
	else if (opCommand == CMD_ERASE) {
		int size = std::stoi(commandVector.at(SIZE));
		return std::make_shared<EraseCommand>(lba, size);
	}
	else if (opCommand == CMD_FLUSH) {
		return std::make_shared<FlushCommand>(-1);	// put garbage value to lba
	}
	else {
		return nullptr;
	}
}

bool
SSDCommandBuilder::isValidOp(const string& opStr) const
{
	if (opStr == CMD_READ 
		|| opStr == CMD_WRITE 
		|| opStr == CMD_ERASE
		|| opStr == CMD_FLUSH ) {
		return true;
	}

	return false;
}

bool
SSDCommandBuilder::isValidValue(const string& valueStr) const
{
	std::regex re("^0x[0-9A-F]{8}$");
	return std::regex_match(valueStr, re);
}

bool
SSDCommandBuilder::isValidLBA(int lba, int size) const
{
	if (size == Size::MIN) {
		return lba >= NandData::LBA::MIN && lba <= NandData::LBA::MAX;
	}

	return lba + size <= NandData::LBA::MAX;
}

bool
SSDCommandBuilder::isValidSize(int size) const
{
	return size >= Size::MIN && size <= Size::MAX;
}