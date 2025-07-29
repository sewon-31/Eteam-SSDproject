#pragma once
#include <iostream>
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

