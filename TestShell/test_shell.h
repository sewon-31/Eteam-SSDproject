#include <sstream>
#include <vector>
#include <string>
#include "ssd_interface.h"
#include "command_parser.h"
#include "file_util.h"
#include "command_executor.h"

class TestShell : public ICommandExecutor{
public:
	TestShell() : ssd(new SSDDriver()) {}
	TestShell(SSDInterface* ssd) :ssd(ssd) {}

	void setSSD(SSDInterface* ssd) {
		this->ssd = ssd;
	}
	void execute() override;
	bool ExecuteCommand(const vector<string>& commandVector);

private:
	SSDInterface* ssd;
	CommandParser commandParser;

	FileUtil fileUtil;
	const string FAIL = "FAIL\n";
};

class TestScript : public ICommandExecutor {
public:
	TestScript(const string& filename) : filename(filename), ssd(new SSDDriver()) {}
	TestScript(SSDInterface* ssd) :ssd(ssd) {}

	void setSSD(SSDInterface* ssd) {
		this->ssd = ssd;
	}

	void setFilename(const string& filename) {
		this->filename = filename;
	}

	void execute() override;
	bool ExecuteCommand(const vector<string>& commandVector);

private:
	SSDInterface* ssd;
	CommandParser commandParser;
	string filename;

	FileUtil fileUtil;
	const string FAIL = "FAIL\n";
};
