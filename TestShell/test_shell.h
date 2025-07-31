#include <sstream>
#include <vector>
#include <string>
#include "ssd_interface.h"
#include "command_parser.h"
#include "file_util.h"

class TestShell {
public:
	TestShell() {}
	TestShell(SSDInterface* ssd) :ssd(ssd) {}

	void setSSD(SSDInterface* ssd) {
		this->ssd = ssd;
	}
	void runShell();
	void runScript(std::string filename);
	bool ExecuteCommand(vector<string> commandVector);

private:
	SSDInterface* ssd;
	CommandParser commandParser;

	FileUtil fileUtil;
	const string FAIL = "FAIL\n";
};
