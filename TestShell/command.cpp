#include "command.h"
#include <iostream>
#include "logger.h"

bool ReadCommand::execute(const std::vector<std::string>& args)
{
	int lba = std::stoi(args.at(ARG_IDX_LBA));
	PRINT_LOG("Executing read to LBA %d", lba);
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
	PRINT_LOG((READ_HEADER + oss.str() + READ_MIDFIX + content).c_str());
	std::cout << READ_HEADER << oss.str() << READ_MIDFIX << content << READ_FOOTER;
}

bool WriteCommand::execute(const std::vector<std::string>& args)
{
	int lba = std::stoi(args.at(ARG_IDX_LBA));
	std::string value = args.at(ARG_IDX_VALUE);
	PRINT_LOG("Executing write to LBA %d with value %s", lba, value);
	std::cout << "Executing write to LBA " << lba << " with value " << value << std::endl;
	write(lba, value);
    return true;
}

void WriteCommand::write(int lba, const std::string& value)
{
	if (ssd == nullptr) return;
	if (lba > MAX_LBA || lba < MIN_LBA)
		return;
	try {
		ssd->write(lba, value);
		PRINT_LOG("LBA [%d]: %#x  Done", lba, value);
		std::cout << "[WRITE] Done" << std::endl;
	}
	catch (SSDExecutionException& e) {
		PRINT_LOG("[WRITE] Fail. %s", e.what());
		std::cout << "[WRITE] Fail" << std::endl;
	}
}

bool FullReadCommand::execute(const std::vector<std::string>& args)
{
	PRINT_LOG("Executing fullread");
	std::cout << "Executing fullread" << std::endl;
	fullRead();
    return true;
}

void FullReadCommand::fullRead()
{
	try {
		for (int addr = MIN_LBA; addr <= MAX_LBA; addr++) {
			ssdReadAndPrint(addr);
		}
	}
	catch (std::exception e) {
		PRINT_LOG("[WRITE] Fail. %s", e.what());
		std::cout << string(e.what());
	}
}

bool FullWriteCommand::execute(const std::vector<std::string>& args)
{
	std::string value = args.at(ARG_IDX_VALUE);
	PRINT_LOG("Executing fullwrite with value");
	std::cout << "Executing fullwrite with value " << value << std::endl;
	fullWrite(value);
    return true;
}

void FullWriteCommand::fullWrite(const std::string& value) {
	if (ssd == nullptr) return;
	try {
		for (int i = MIN_LBA; i <= MAX_LBA; i++)
			ssd->write(i, value);
		PRINT_LOG("[FULL_WRITE] Done");
		std::cout << "[FULL_WRITE] Done" << std::endl;
	}
	catch (SSDExecutionException& e) {
		PRINT_LOG("[FULL_WRITE] Fail. %s", e.what());
		std::cout << "[FULL_WRITE] Fail" << std::endl;
	}
}

bool ExitCommand::execute(const std::vector<std::string>& args)
{
	PRINT_LOG("bye bye");
    return false;
}

bool HelpCommand::execute(const std::vector<std::string>& args)
{
	PRINT_LOG("print help");
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

	std::cout << "\terase <address> <size>\n";
	std::cout << "\t\tErase contents starting from the specified LBA address for size.\n\n";

	std::cout << "\terase_range <start address> <end address>\n";
	std::cout << "\t\tErase contents from Start LBA address to End LBA address (inclusive).\n\n";

	std::cout << "\tflush\n";
	std::cout << "\t\tExecute all commands in the command buffer and clear the buffer.\n\n";

	std::cout << "\thelp\n";
	std::cout << "\t\tShow this help message.\n\n";

	std::cout << "\texit\n";
	std::cout << "\t\tExit the program.\n\n";

	std::cout << "Address / Value format:\n";
	std::cout << "\t<address> : Decimal integer (e.g. 16, 255)\n";
	std::cout << "\t<value>   : 32-bit hexadecimal number\n";
	std::cout << "\t\tMust start with '0x'\n";
	std::cout << "\t\tMust contain exactly 8 hex digits (0-9, A-F)\n";
	std::cout << "\t\tExample: 0x12345678, 0xDEADBEEF\n";
	std::cout << "\t<size>   : Decimal integer (e.g. 16, 255)\n\n";
    return true;
}

bool EraseCommand::execute(const std::vector<std::string>& args)
{
	int lba = std::stoi(args.at(ARG_IDX_LBA));
	int size = std::stoi(args.at(ARG_IDX_SIZE));
	std::cout << "Executing erase" << std::endl;
	PRINT_LOG("Executing erase");
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

		parseSizeAndErase(lba, size);
		PRINT_LOG("[ERASE] Done");
		std::cout << "[ERASE] Done" << std::endl;
	}
	catch (SSDExecutionException& e) {
		PRINT_LOG("[ERASE] Fail. %s", e.what());
		std::cout << "[ERASE] Fail" << std::endl;
	}
}

void EraseCommand::parseSizeAndErase(int lba, int size)
{
	while (size > 0) {
		if (size > MAX_ERASE_SIZE) {
			ssd->erase(lba, MAX_ERASE_SIZE);
			lba = lba + MAX_ERASE_SIZE;
			size = size - MAX_ERASE_SIZE;
		}
		else {
			ssd->erase(lba, size);
			size = 0;
		}
	}
}

bool EraseRangeCommand::execute(const std::vector<std::string>& args)
{
	int startLba = std::stoi(args.at(ARG_IDX_START_LBA));
	int endLba = std::stoi(args.at(ARG_IDX_END_LBA));
	std::cout << "Executing erase_range" << std::endl;
	PRINT_LOG("Executing erase_range");
	eraseRange(startLba, endLba);
	return true;
}

void EraseRangeCommand::eraseRange(int startLba, int endLba) {
	if (startLba > endLba)
		std::swap(startLba, endLba);
	try {
		parseSizeAndErase(startLba, endLba - startLba + 1);
		PRINT_LOG("[ERASE RANGE] Done");
		std::cout << "[ERASE RANGE] Done" << std::endl;
	}
	catch (SSDExecutionException& e) {
		PRINT_LOG("[ERASE RANGE] Fail. %s", e.what());
		std::cout << "[ERASE RANGE] Fail" << std::endl;
	}
}

bool FlushCommand::execute(const std::vector<std::string>& args)
{
	PRINT_LOG("Executing flush");
	std::cout << "Executing flush" << std::endl;
	flush();
	return true;
}

void FlushCommand::flush() {
	try {
		ssd->flush();
		PRINT_LOG("[FLUSH] Done");
		std::cout << "[FLUSH] Done" << std::endl;

	}
	catch (SSDExecutionException& e) {
		PRINT_LOG("[FLUSH] Fail. %s", e.what());
		std::cout << "[FLUSH] Fail" << std::endl;
	}
}
