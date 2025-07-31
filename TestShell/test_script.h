#pragma once

#include <string>

#include "ssd_interface.h"

class ScriptsCommand {
public:
	ScriptsCommand(SSDInterface* device) : ssd(device) {}
	virtual bool run() = 0;

protected:
	int generateRandomIntValue() const;
	std::string intToHexString(int hexValue) const;
	bool readCompare(int address, std::string hexValue) const;

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