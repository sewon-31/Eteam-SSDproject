#include <sstream>
#include <vector>
#include <string>
#include "ssd_interface.h"
#include "command_parser.h"

class TestShell {
public:
	TestShell() {}
	TestShell(SSDInterface* ssd) :ssd(ssd) {}
	void setSSD(SSDInterface* ssd) {
		this->ssd = ssd;
	}
	void runShell();
	void read(int lba);
	void fullRead();
	void sseReadAndPrint(int addr);
	void write(int lba, std::string value);
	void fullWrite(std::string value);
	void help();
private:
	SSDInterface* ssd;
	CommandParser commandParser;
	const int MAX_LBA = 100;
	const string READ_HEADER = "[Read] LBA ";
	const string READ_MIDFIX = " : ";
	const string READ_FOOTER = "\n";
};
