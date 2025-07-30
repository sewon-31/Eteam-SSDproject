#pragma once
#include <iostream>
#include <fstream>
#include "gmock/gmock.h"

using std::string;

class SSDInterface {
public:
	virtual void write(int lba, string value) = 0;
	virtual string read(int lba) = 0;
};

class MockSSD : public SSDInterface {
public:
	MOCK_METHOD(void, write, (int lba, string value), (override));
	MOCK_METHOD(string, read, (int lba), (override));
};

class SSDDriver : public SSDInterface {
public:
	virtual bool runExe(const string& command) {
		int ret = system(command.c_str());

		if (ret == 0) return true;
		return false;
	}
	void write(int lba, string value) override {

	}
	string read(int lba) override {
		string command = "\"SSD.exe >nul 2>&1\"";
		if (runExe(command) == false) { throw std::runtime_error("There is no SSD.exe\n"); }

		string content;
		std::ifstream file(SSD_READ_RESULT);
		std::getline(file, content);
		file.close();

		return content;
	}
private:
	const string SSD_READ_RESULT = "ssd_output.txt";
};

class MockSSDDriver : public SSDDriver {
public:
	MOCK_METHOD(bool, runExe, (const string& command), (override));
};