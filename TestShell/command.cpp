#include "command.h"
#include <iostream>

bool ReadCommand::execute(const std::vector<std::string>& args)
{
    return false;
}

bool WriteCommand::execute(const std::vector<std::string>& args)
{
    return false;
}

bool FullReadCommand::execute(const std::vector<std::string>& args)
{
    return false;
}

bool FullWriteCommand::execute(const std::vector<std::string>& args)
{
    return false;
}

bool ExitCommand::execute(const std::vector<std::string>& args)
{
    return false;
}

bool HelpCommand::execute(const std::vector<std::string>& args)
{
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
    return true;
}

bool TestScript1Command::execute(const std::vector<std::string>& args)
{
    return false;
}

bool TestScript2Command::execute(const std::vector<std::string>& args)
{
    return false;
}

bool TestScript3Command::execute(const std::vector<std::string>& args)
{
    return false;
}
