#pragma once

#include <string>

#include "ssd_interface.h"
#include "command.h"

class ScriptsCommand : public Command{
public:
	ScriptsCommand(SSDInterface* device) : ssd(device) {}
	bool execute(const std::vector<std::string>& args) override { 
		if (run()) result = "PASS";
		std::cout << result << '\n';
		return true; 
	};
	virtual bool run() = 0;

protected:
	int generateRandomIntValue() const;
	std::string intToHexString(int hexValue) const;
	bool readCompare(int address, std::string hexValue) const;

	string result = "FAIL";
	SSDInterface* ssd;
};


class ScriptsFullWriteAndReadCompare : public ScriptsCommand {
public:
	ScriptsFullWriteAndReadCompare(SSDInterface* device) : ScriptsCommand(device) {}
	bool run() override;
	
private:
	const static int MAX_LBA = 100;
	const static int OPERATE_COUNT_PER_LOOP = 5;
	const static int LOOP_COUNT = MAX_LBA / OPERATE_COUNT_PER_LOOP;
};

class ScriptsPartialLBAWrite : public ScriptsCommand {
public:
	ScriptsPartialLBAWrite(SSDInterface* device) : ScriptsCommand(device) {}
	bool run() override;
};

class ScriptsWriteReadAging : public ScriptsCommand {
public:
	ScriptsWriteReadAging(SSDInterface* device) : ScriptsCommand(device) {}
	bool run() override;
};