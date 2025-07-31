#include "command.h"
#include <iostream>

bool ReadCommand::execute(const std::vector<std::string>& args)
{
	int lba = std::stoi(args.at(0));
	std::cout << "Executing read from LBA " << lba << std::endl;
	read(lba);
    return true;
}
void ReadCommand::read(int lba)
{
	try {
		ssdReadAndPrint(lba);
	}
	catch (std::exception e) {
		std::cout << string(e.what());
	}
}
void ReadCommand::ssdReadAndPrint(int addr)
{
	std::string content = ssd->read(addr);
	std::ostringstream oss;
	oss << std::setw(2) << std::setfill('0') << addr;

	std::cout << READ_HEADER << oss.str() << READ_MIDFIX << content << READ_FOOTER;
}

bool WriteCommand::execute(const std::vector<std::string>& args)
{
	int lba = std::stoi(args.at(0));
	std::string value = args.at(1);
	std::cout << "Executing write to LBA " << lba << " with value " << value << std::endl;
	write(lba, value);
    return true;
}

void WriteCommand::write(int lba, std::string value)
{
	if (ssd == nullptr) return;
	if (lba >= 100 || lba < 0)
		return;
	try {
		ssd->write(lba, value);
		std::cout << "[WRITE] Done" << std::endl;
	}
	catch (SSDExecutionException& e) {
		std::cout << "[WRITE] Fail" << std::endl;
	}
}

bool FullReadCommand::execute(const std::vector<std::string>& args)
{
	std::cout << "Executing fullread" << std::endl;
	fullRead();
    return true;
}

void FullReadCommand::fullRead()
{
	try {
		for (int addr = 0; addr < MAX_LBA; addr++) {
			ssdReadAndPrint(addr);
		}
	}
	catch (std::exception e) {
		std::cout << string(e.what());
	}
}

bool FullWriteCommand::execute(const std::vector<std::string>& args)
{
	std::string value = args.at(0);
	std::cout << "Executing fullwrite with value " << value << std::endl;
	fullWrite(value);
    return true;
}

void FullWriteCommand::fullWrite(std::string value) {
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

bool EraseCommand::execute(const std::vector<std::string>& args)
{
	int lba = std::stoi(args.at(0));
	int size = std::stoi(args.at(1));
	std::cout << "Executing erase" << std::endl;
	erase(lba, size);
	return true;
}

void EraseCommand::erase(int lba, int size) {
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

bool EraseRangeCommand::execute(const std::vector<std::string>& args)
{
	int startLba = std::stoi(args.at(0));
	int endLba = std::stoi(args.at(1));
	std::cout << "Executing erase_range" << std::endl;
	eraseRange(startLba, endLba);
	return true;
}

void EraseRangeCommand::eraseRange(int startLba, int endLba) {
	if (startLba > endLba)
		std::swap(startLba, endLba);
	try {
		erase(startLba, endLba - startLba + 1);
		std::cout << "[ERASE RANGE] Done" << std::endl;
	}
	catch (SSDExecutionException& e) {
		std::cout << "[ERASE RANGE] Fail" << std::endl;
	}
}

bool FlushCommand::execute(const std::vector<std::string>& args)
{
	std::cout << "Executing flush" << std::endl;
	flush();
	return true;
}

void FlushCommand::flush() {
	try {
		ssd->flush();
		std::cout << "[FLUSH] Done" << std::endl;

	}
	catch (SSDExecutionException& e) {
		std::cout << "[FLUSH] Fail" << std::endl;
	}
}
