#pragma once
#include "gmock/gmock.h"
#include "mock_ssd.h"
#include <string>

using namespace testing;
using namespace std;

//class MockTestShell : public TestShell {
//public:
//	MOCK_METHOD(void, help, (int lba));
//	MOCK_METHOD(void, write, (int lba, std::string value));
//	MOCK_METHOD(void, fullRead, ());
//	MOCK_METHOD(void, fullWrite, (std::string value));
//	MOCK_METHOD(void, help, ());
//private:
//};
//
class TestShellCommandOperatorFixture : public Test {
public:
	TestShell app;

	void SetUp() override {
		app.setSSD(&mockSSD);
	}
public:
	MockSSD mockSSD;
};

TEST_F(TestShellCommandOperatorFixture, Read) {
	vector<string> commandVector = { "read", "3" };

	EXPECT_CALL(mockSSD, read(_))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, Write) {
	vector<string> commandVector = { "write", "3", "0xAAAAAAAA"};

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, FullRead) {
	vector<string> commandVector = { "fullread" };

	EXPECT_CALL(mockSSD, read(_))
		.Times(100);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, FullWrite) {
	vector<string> commandVector = { "fullwrite", "0xAAAAAAAA"};

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(100);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, Erase) {
	vector<string> commandVector = { "erase", "5", "2"};

	EXPECT_CALL(mockSSD, erase(5, 2))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, EraseRange) {
	vector<string> commandVector = { "erase_range", "1", "4" };

	EXPECT_CALL(mockSSD, erase(1, 4))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, Flush) {
	vector<string> commandVector = { "flush" };

	EXPECT_CALL(mockSSD, flush())
		.Times(1);

	app.ExecuteCommand(commandVector);
}