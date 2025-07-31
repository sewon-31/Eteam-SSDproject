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
	virtual void erase(int lba, int size);
	virtual void eraseRange(int startLba, int endLba);
	virtual void flush();

private:
	SSDInterface* ssd;
	CommandParser commandParser;

	FileUtil fileUtil;
	const string PASS = "PASS\n";
	const string FAIL = "FAIL";
};
