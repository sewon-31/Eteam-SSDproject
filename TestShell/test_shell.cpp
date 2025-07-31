#include "test_shell.h"
#include "command_parser.h"
#include "test_script.h"
#include "command.h"

#include <iostream>

using std::cout;
bool TestShell::ExecuteCommand(vector<string> commandVector)
{
    std::string opCommand = commandVector.at(0);

    string result = "FAIL";

    if (opCommand == CommandParser::CMD_EXIT) {
        Command* command = new ExitCommand();
        if (!command->execute(commandVector)) return false;
    }
    else if (opCommand == CommandParser::CMD_HELP) {
        Command* command = new HelpCommand();
        if (!command->execute(commandVector)) return false;
    }
    else if (opCommand == CommandParser::CMD_WRITE) {
        Command* command = new WriteCommand(ssd);
        if (!command->execute(commandVector)) return false;
    }
    else if (opCommand == CommandParser::CMD_READ) {
        Command* command = new ReadCommand(ssd);
        if (!command->execute(commandVector)) return false;
    }
    else if (opCommand == CommandParser::CMD_FULLWRITE) {
        Command* command = new FullWriteCommand(ssd);
        if (!command->execute(commandVector)) return false;
    }
    else if (opCommand == CommandParser::CMD_FULLREAD) {
        Command* command = new FullReadCommand(ssd);
        if (!command->execute(commandVector)) return false;
    }
    else if (opCommand == CommandParser::CMD_ERASE) {
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
    else if (opCommand == CommandParser::CMD_SCRIPT1 || opCommand == CommandParser::CMD_SCRIPT1_NAME) {
        Command* command = new ScriptsFullWriteAndReadCompare(ssd);
        if (!command->execute(commandVector)) return false;
 }
    else if (opCommand == CommandParser::CMD_SCRIPT2 || opCommand == CommandParser::CMD_SCRIPT2_NAME) {
        Command* command = new ScriptsPartialLBAWrite(ssd);
        if (!command->execute(commandVector)) return false;
    }
    else if (opCommand == CommandParser::CMD_SCRIPT3 || opCommand == CommandParser::CMD_SCRIPT3_NAME) {
        Command* command = new ScriptsWriteReadAging(ssd);
        if (!command->execute(commandVector)) return false;
    }
    else {
        std::cout << "[Error] Unknown command: " << opCommand << std::endl;
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
    std::fstream scriptListFile(filename, std::ios::in);

    if (scriptListFile.is_open() == false) {
        std::cout << "[Error] Invalid File Name." << std::endl;
        return;
    }
    
    std::string opCommand;
    ScriptsCommand* scriptCommand;

    while (std::getline(scriptListFile, opCommand)) {
        cout << opCommand <<"   ___   Run ... ";

        if (opCommand == CommandParser::CMD_SCRIPT1 || opCommand == CommandParser::CMD_SCRIPT1_NAME) {
            scriptCommand  = new ScriptsFullWriteAndReadCompare(ssd);  
        }
        else if (opCommand == CommandParser::CMD_SCRIPT2 || opCommand == CommandParser::CMD_SCRIPT2_NAME) {
            scriptCommand = new ScriptsPartialLBAWrite(ssd);
        }
        else if (opCommand == CommandParser::CMD_SCRIPT3 || opCommand == CommandParser::CMD_SCRIPT3_NAME) {
            scriptCommand = new ScriptsWriteReadAging(ssd);         
        }
        else {
            cout << "FAIL!\n";
            break;
        }

        if (scriptCommand->run() == false) {
            cout << "FAIL!\n";
            break;
        }

        cout << "PASS\n";
    }

    scriptListFile.close();
}