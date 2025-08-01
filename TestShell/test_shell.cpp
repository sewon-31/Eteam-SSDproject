#include "test_shell.h"
#include "command_parser.h"
#include "test_script.h"
#include "command_factory.h"
#include <iostream>
#include "logger.h"

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
        PRINT_LOG("[Error] Unknown command: ");
        std::cout << "[Error] Unknown command: " << commandVector.at(0) << std::endl;
        return true;
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

        PRINT_LOG(inputLine.c_str());
        commandParser.setCommand(inputLine);

        if (!commandParser.isValidCommand()) {
            PRINT_LOG("[Error] Invalid command or arguments.");
            std::cout << "[Error] Invalid command or arguments." << std::endl;
            continue;
        }

        if (false == ExecuteCommand(commandParser.getCommandVector())) return;
    }
}

void TestShell::runScript(std::string filename)
{
    if (fileUtil.fileExists(filename) == false) {
        PRINT_LOG("[Error] Invalid File Name.");
        std::cout << "[Error] Invalid File Name." << std::endl;
        return;
    }

    vector<std::string> commandList;
    fileUtil.readAllLines(filename, commandList);

    for (auto opCommand : commandList) {
        vector<string> commandVector = { opCommand };

        string log = opCommand + "   ___   Run ... ";
        cout << log;
        PRINT_LOG(log.c_str());

        std::unique_ptr<Command> command = CommandFactory::createScriptCommand(commandVector, ssd);

       if(command == nullptr) {
            cout << FAIL;
            break;
        }

        if (command->execute(commandVector) == false) break;
    }
}