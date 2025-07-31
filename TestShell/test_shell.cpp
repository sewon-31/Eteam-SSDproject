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
        std::string value = commandVector.at(1);
        std::cout << "Executing fullwrite with value " << value << std::endl;
        fullWrite(value);
    }
    else if (opCommand == CommandParser::CMD_FULLREAD) {
        std::cout << "Executing fullread" << std::endl;
        fullRead();
    }
    else if (opCommand == CommandParser::CMD_ERASE) {
        int lba = std::stoi(commandVector.at(1));
        int size = std::stoi(commandVector.at(2));
        std::cout << "Executing erase" << std::endl;
        erase(lba, size);
    }
    else if (opCommand == CommandParser::CMD_ERASE_RANGE) {
        int startLba = std::stoi(commandVector.at(1));
        int endLba = std::stoi(commandVector.at(2));
        std::cout << "Executing erase_range" << std::endl;
        eraseRange(startLba, endLba);
    }
    else if (opCommand == CommandParser::CMD_SCRIPT1 || opCommand == CommandParser::CMD_SCRIPT1_NAME) {
        ScriptsCommand* scriptCommand = new ScriptsFullWriteAndReadCompare(ssd);

        std::cout << "Running script 1: FullWriteAndReadCompare" << std::endl;
        if (scriptCommand->run()) result = "TRUE";

        std::cout << result << '\n';
    }
    else if (opCommand == CommandParser::CMD_SCRIPT2 || opCommand == CommandParser::CMD_SCRIPT2_NAME) {
        ScriptsCommand* scriptCommand = new ScriptsPartialLBAWrite(ssd);
        
        std::cout << "Running script 2: PartialLBAWrite" << std::endl;
        if (scriptCommand->run()) result = "TRUE";

        std::cout << result << '\n';
    }
    else if (opCommand == CommandParser::CMD_SCRIPT3 || opCommand == CommandParser::CMD_SCRIPT3_NAME) {
        ScriptsCommand* scriptCommand = new ScriptsWriteReadAging(ssd);

        std::cout << "Running script 3: WriteReadAging" << std::endl;
        if (scriptCommand->run()) result = "TRUE";

        std::cout << result << '\n';
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

void TestShell::fullRead()
{
    try {
	    for (int addr = 0; addr < MAX_LBA; addr++) {       
            ssdReadAndPrint(addr);
	    }
    }
    catch (std::exception e) {
        cout << string(e.what());
    }
}

void TestShell::ssdReadAndPrint(int addr)
{
    std::string content = ssd->read(addr);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << addr;

    cout << READ_HEADER << oss.str() << READ_MIDFIX << content << READ_FOOTER;
}

void TestShell::fullWrite(std::string value) {
    if (ssd == nullptr) return;
	try {
		for (int i = 0; i < 100; i++)
			ssd->write(i, value);
		std::cout << "[FULL_WRITE] Done" << std::endl;
	}
	catch (SSDExecutionException& e) {
		std::cout << "[FULL_WRITE] Fail" << std::endl;
	}
}

void TestShell::erase(int lba, int size) {
    try {
        // lba : 0 <= lba < 100, size : -INT_MAX ~ INT_MAX
        if (size < 0) {
            if (size + lba < 0) {
                size = lba + 1;
                lba = 0;
            }
            else {
                lba = size + lba + 1;
                size = std::abs(size);
            }
        }
        else if (size + lba > 100) {
            size = 100 - lba;
        }

        ssd->erase(lba, size);
        std::cout << "[ERASE] Done" << std::endl;
    }
    catch (SSDExecutionException& e) {
        std::cout << "[ERASE] Fail" << std::endl;
    }
}

void TestShell::eraseRange(int startLba, int endLba) {
    if (startLba > endLba)
        std::swap(startLba, endLba);
    try {
        ssd->eraseRange(startLba, endLba);
        std::cout << "[ERASE RANGE] Done" << std::endl;
    }
    catch (SSDExecutionException& e) {
        std::cout << "[ERASE RANGE] Fail" << std::endl;
    }
}