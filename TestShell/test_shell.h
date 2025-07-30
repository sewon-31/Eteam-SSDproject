#include <sstream>
#include <vector>
#include <string>
#include "ssd_interface.h"
#include "command_parser.h"
#include <vector>

class TestShell {
public:
	TestShell() {}
	TestShell(SSDInterface* ssd) :ssd(ssd) {}
	void setSSD(SSDInterface* ssd) {
		this->ssd = ssd;
	}
	void runShell();
	bool ExecuteCommand(vector<string> commandVector);
	virtual void read(int lba);
	virtual void fullRead();

	void ssdReadAndPrint(int addr);
	virtual void write(int lba, std::string value);
	virtual void fullWrite(std::string value);
	virtual void help();
private:
	SSDInterface* ssd;
	CommandParser commandParser;
	const int MAX_LBA = 100;
	const string READ_HEADER = "[Read] LBA ";
	const string READ_MIDFIX = " : ";
	const string READ_FOOTER = "\n";
};
