#pragma once
#include "test_shell.h"

class MockSSD : public SSDInterface {
public:
	MOCK_METHOD(void, write, (int lba, string value), (override));
	MOCK_METHOD(string, read, (int lba), (override));
};

class MockSSDDriver : public SSDDriver {
public:
	MOCK_METHOD(bool, runExe, (const string& command), (override));
};