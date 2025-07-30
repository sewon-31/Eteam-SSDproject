#include "test_shell.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

using std::cout;

void TestShell::runShell()
{
    std::string inputLine;

    while (true) {
        std::cout << "shell > ";
        std::getline(std::cin, inputLine);

        // Skip empty input

        if (inputLine.empty()) continue;

        // Tokenize command
        std::istringstream ss(inputLine);
        std::string command;
        ss >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "help") {
            help();
        }
        else if (command == "write") {
            int lba;
            std::string value;
            ss >> lba >> value;
            std::cout << "Executing write to LBA " << lba << " with value " << value << std::endl;
            // Call write(lba, value);
        }
        else if (command == "read") {
            int lba;
            ss >> lba;
            std::cout << "Executing read from LBA " << lba << std::endl;
            // Call read(lba);
        }
        else if (command == "fullwrite") {
            std::string value;
            ss >> value;
            std::cout << "Executing fullwrite with value " << value << std::endl;
            // Call fullwrite(value);
        }
        else if (command == "fullread") {
            std::cout << "Executing fullread" << std::endl;
            // Call fullread();
        }
        else if (command == "1_" || command == "1_FullWriteAndReadCompare") {
            std::cout << "Running script 1: FullWriteAndReadCompare" << std::endl;
            // Call TestScript1::Run();
        }
        else if (command == "2_" || command == "2_PartialLBAWrite") {
            std::cout << "Running script 2: PartialLBAWrite" << std::endl;
            // Call TestScript2::Run();
        }
        else if (command == "3_" || command == "3_WriteReadAging") {
            std::cout << "Running script 3: WriteReadAging" << std::endl;
            // Call TestScript3::writeReadAging();
        }
        else {
            std::cout << "[Error] Unknown command: " << command << std::endl;
        }
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