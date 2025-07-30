#include "test_shell.h"
#include "command_parser.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using std::cout;
void TestShell::ExecuteCommand(vector<string> commandVector)
{
    std::string opCommand = commandVector.at(0);

    if (opCommand == CommandParser::CMD_EXIT) {
        return;
    }
    else if (opCommand == CommandParser::CMD_HELP) {
        help();
    }
    else if (opCommand == CommandParser::CMD_WRITE) {
        int lba = std::stoi(commandVector.at(1));
        std::string value = commandVector.at(2);
        std::cout << "Executing write to LBA " << lba << " with value " << value << std::endl;
        write(lba, value);
    }
    else if (opCommand == CommandParser::CMD_READ) {
        int lba = std::stoi(commandVector.at(1));
        std::cout << "Executing read from LBA " << lba << std::endl;
        read(lba);
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
    else if (opCommand == CommandParser::CMD_SCRIPT1 || opCommand == CommandParser::CMD_SCRIPT1_NAME) {
        std::cout << "Running script 1: FullWriteAndReadCompare" << std::endl;
        // TestScript1::Run();
    }
    else if (opCommand == CommandParser::CMD_SCRIPT2 || opCommand == CommandParser::CMD_SCRIPT2_NAME) {
        std::cout << "Running script 2: PartialLBAWrite" << std::endl;
        // TestScript2::Run();
    }
    else if (opCommand == CommandParser::CMD_SCRIPT3 || opCommand == CommandParser::CMD_SCRIPT3_NAME) {
        std::cout << "Running script 3: WriteReadAging" << std::endl;
        // TestScript3::writeReadAging();
    }
    else {
        std::cout << "[Error] Unknown command: " << opCommand << std::endl;
    }
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

        ExecuteCommand(commandParser.getCommandVector());
    }
}

void TestShell::read(int lba) {
	std::string content = ssd->read(lba);

	std::ostringstream oss;
	oss << std::setw(2) << std::setfill('0') << lba;

	cout << READ_HEADER << oss.str() << READ_MIDFIX << content << READ_FOOTER;
}

void TestShell::fullRead()
{
	for (int addr = 0; addr < MAX_LBA; addr++) {
		read(addr);
	}
}

void TestShell::write(int lba, std::string value) {
	if (ssd == nullptr) return;
	if (lba >= 100 || lba < 0)
		return;
	ssd->write(lba, value);
	std::cout << "[WRITE] Done" << std::endl;
}

void TestShell::fullWrite(std::string value) {
	for (int i = 0; i < 100; i++)
		ssd->write(i, value);
}

void TestShell::help() {
	std::cout << "Team: Easiest\n";
	std::cout << "Member: Sewon Joo, Dokyeong Kim, Nayoung Yoon, Seungah Lim, Jaeyeong Jeon, Insang Cho, Dooyeun Hwang\n\n";

	std::cout << "Available commands:\n\n";

	std::cout << "\twrite <address> <value>\n";
	std::cout << "\t\tWrite a 32-bit value to the specified address.\n\n";

	std::cout << "\tread <address>\n";
	std::cout << "\t\tRead a 32-bit value from the specified decimal address.\n\n";

	std::cout << "\tfullwrite <value>\n";
	std::cout << "\t\tFill the entire memory region with the specified 32-bit hex value.\n\n";

	std::cout << "\tfullread\n";
	std::cout << "\t\tRead and display the entire memory region.\n\n";

	std::cout << "\thelp\n";
	std::cout << "\t\tShow this help message.\n\n";

	std::cout << "\texit\n";
	std::cout << "\t\tExit the program.\n\n";

	std::cout << "Address / Value format:\n";
	std::cout << "\t<address> : Decimal integer (e.g., 16, 255)\n";
	std::cout << "\t<value>   : 32-bit hexadecimal number\n";
	std::cout << "\t\tMust start with '0x'\n";
	std::cout << "\t\tMust contain exactly 8 hex digits (0-9, A-F)\n";
	std::cout << "\t\tExample: 0x12345678, 0xDEADBEEF\n\n";
}