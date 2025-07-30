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

class SSDExecutionException : public std::exception {
public:
	explicit SSDExecutionException(const std::string& msg)
		: message_(msg) {
	}

	const char* what() const noexcept override {
		return message_.c_str();
	}

private:
	std::string message_;
};

class MockSSD : public SSDInterface {
public:
	MOCK_METHOD(void, write, (int lba, string value), (override));
	MOCK_METHOD(string, read, (int lba), (override));
};

class SSDDriver : public SSDInterface {
public:
	virtual bool runExe(const string& command) {
		int isFail = system(command.c_str());

		if (isFail) return false;
		return true;
	}
	void write(int lba, string value) override {
		string ssdCmd = "\"ssd W " + std::to_string(lba) + " " + value;
		if (runExe(ssdCmd) == false) {
			throw SSDExecutionException("Execution failed: " + ssdCmd);
		}
	}
	string read(int lba) override {
		string command = "\"ssd R " + std::to_string(lba) + " >nul 2>&1\"";
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