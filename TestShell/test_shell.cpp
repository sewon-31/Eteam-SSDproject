#include "test_shell.h"
#include "command_parser.h"
#include "test_script.h"
#include "command.h"

#include <iostream>

using std::cout;
bool TestShell::ExecuteCommand(vector<string> commandVector)
{
    std::string opCommand = commandVector.at(0);

    std::unique_ptr<Command> command = CommandFactory::createCommand(commandVector, ssd);

    if (command) {
        std::vector<std::string> args(commandVector.begin() + 1, commandVector.end());
        return command->execute(args);
    }
    else {
        std::cout << "[Error] Unknown command: " << commandVector.at(0) << std::endl;
        return true;
    }

    if (opCommand == CommandParser::CMD_ERASE) {
        Command* command = new EraseCommand(ssd);
        if (!command->execute(commandVector)) return false;
    }
    else if (opCommand == CommandParser::CMD_ERASE_RANGE) {
        Command* command = new EraseRangeCommand(ssd);
        if (!command->execute(commandVector)) return false;
    }
    else if (opCommand == CommandParser::CMD_FLUSH) {
        Command* command = new FlushCommand(ssd);
        if (!command->execute(commandVector)) return false;
    }

    return true;
}

void TestShell::runShell()
{
    std::string inputLine;

    while (true) {
        std::cout << "shell > ";
        std::getline(std::cin, inputLine);

        if (inputLine.empty()) continue;

        commandParser.setCommand(inputLine);

        if (!commandParser.isValidCommand()) {
            std::cout << "[Error] Invalid command or arguments." << std::endl;
            continue;
        }

        if (false == ExecuteCommand(commandParser.getCommandVector())) return;
    }
}

void TestShell::runScript(std::string filename)
{
    if (fileUtil.fileExists(filename) == false) {
        std::cout << "[Error] Invalid File Name." << std::endl;
        return;
    }

    vector<std::string> commandList;
    fileUtil.readAllLines(filename, commandList);

    for (auto opCommand : commandList) {
        Command* command;
        vector<string> commandVector = { opCommand };

        string log = opCommand + "   ___   Run ... ";
        cout << log;

        if (opCommand == CommandParser::CMD_SCRIPT1 || opCommand == CommandParser::CMD_SCRIPT1_NAME) {
            command = new ScriptsFullWriteAndReadCompare(ssd);
        }
        else if (opCommand == CommandParser::CMD_SCRIPT2 || opCommand == CommandParser::CMD_SCRIPT2_NAME) {
            command = new ScriptsPartialLBAWrite(ssd);
        }
        else if (opCommand == CommandParser::CMD_SCRIPT3 || opCommand == CommandParser::CMD_SCRIPT3_NAME) {
            command = new ScriptsWriteReadAging(ssd);
        }
        else {
            cout << FAIL;
            break;
        }

        if (command->execute(commandVector) == false) break;
    }
}