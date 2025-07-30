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

class SSDDriver : public SSDInterface {
public:
	virtual bool runExe(const string& command) {
		int isFail = system(command.c_str());

		if (isFail) return false;
		return true;
	}
	void write(int lba, string value) override;
	string read(int lba) override;

private:
	const string SSD_READ_RESULT = "ssd_output.txt";
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
