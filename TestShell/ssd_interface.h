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
	string read(int lba) override {
		//string command = ".\program.exe ssd R " + lba;
		//system(command.c_str());

		string content;
		std::ifstream file(SSD_READ_RESULT);
		std::getline(file, content);
		file.close();

		return content;
	}
private:
	const string SSD_READ_RESULT = "ssd_output.txt";
};
