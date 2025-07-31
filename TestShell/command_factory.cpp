#include "command_factory.h"
#include "command_parser.h"
#include "test_script.h"

std::unique_ptr<Command> CommandFactory::createCommand(const std::vector<std::string>& commandVector, SSDInterface* ssd) {
	if (commandVector.empty()) {
		return nullptr;
	}

	std::string opCommand = commandVector.at(0);

	if (opCommand == CommandParser::CMD_EXIT) {
		return std::make_unique<ExitCommand>();
	}
	else if (opCommand == CommandParser::CMD_HELP) {
		return std::make_unique<HelpCommand>();
	}
	else if (opCommand == CommandParser::CMD_WRITE) {
		return std::make_unique<WriteCommand>(ssd);
	}
	else if (opCommand == CommandParser::CMD_READ) {
		return std::make_unique<ReadCommand>(ssd);
	}
	else if (opCommand == CommandParser::CMD_FULLWRITE) {
		return std::make_unique<FullWriteCommand>(ssd);
	}
	else if (opCommand == CommandParser::CMD_FULLREAD) {
		return std::make_unique<FullReadCommand>(ssd);
	}
	else if (opCommand == CommandParser::CMD_ERASE) {
		//
	}
	else if (opCommand == CommandParser::CMD_ERASE_RANGE) {
		//
	}
	else if (opCommand == CommandParser::CMD_FLUSH) {
		//
	}
	else if (opCommand == CommandParser::CMD_SCRIPT1 || opCommand == CommandParser::CMD_SCRIPT1_NAME) {
		return std::make_unique<ScriptsFullWriteAndReadCompare>(ssd);
	}
	else if (opCommand == CommandParser::CMD_SCRIPT2 || opCommand == CommandParser::CMD_SCRIPT2_NAME) {
		return std::make_unique<ScriptsPartialLBAWrite>(ssd);
	}
	else if (opCommand == CommandParser::CMD_SCRIPT3 || opCommand == CommandParser::CMD_SCRIPT3_NAME) {
		return std::make_unique<ScriptsWriteReadAging>(ssd);
	}

	return nullptr;
}