#pragma once
#include "test_shell.h"

class MockSSD : public SSDInterface {
public:
	MOCK_METHOD(void, write, (int lba, string value), (override));
	MOCK_METHOD(string, read, (int lba), (override));
	MOCK_METHOD(void, erase, (int lba, int size), (override));
<<<<<<< HEAD
	MOCK_METHOD(void, flush, (), (override));
=======
	MOCK_METHOD(void, eraseRange, (int startLba, int endLba), (override));
>>>>>>> 2d2ef5e ([feature] TestShell : erase_range implementation in SSD Interface)
};

class MockSSDDriver : public SSDDriver {
public:
	MOCK_METHOD(bool, runExe, (const string& command), (override));
};