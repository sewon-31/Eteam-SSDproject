#pragma once
#include "gmock/gmock.h"
#include "mock_ssd.h"
#include <string>

using namespace testing;
using namespace std;

class TestShellCommandOperatorFixture : public Test {
public:
	TestShell app;

	void SetUp() override {
		app.setSSD(&mockSSD);
	}
public:
	MockSSD mockSSD;
	string testData = "0x00012345";
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
TEST_F(TestShellCommandOperatorFixture, TestScript1) {
	vector<string> commandVector = { "1_" };

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(100);
	EXPECT_CALL(mockSSD, read(_))
		.Times(100)
		.WillRepeatedly(Return(testData));

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, TestScript2) {
	vector<string> commandVector = { "2_" };

	EXPECT_CALL(mockSSD, write(_, _))
		.WillRepeatedly(Return());

	EXPECT_CALL(mockSSD, read(_))
		.WillRepeatedly(Return(testData));

	app.ExecuteCommand(commandVector);
}
TEST_F(TestShellCommandOperatorFixture, TestScript3) {
	vector<string> commandVector = { "3_" };

	std::string val0 = "0x00001111";
	std::string val99 = "0x00009999";

	EXPECT_CALL(mockSSD, write(0, val0))
		.Times(200);
	EXPECT_CALL(mockSSD, write(99, val99))
		.Times(200);
	EXPECT_CALL(mockSSD, read(0))
		.Times(200)
		.WillRepeatedly(Return(val0));
	EXPECT_CALL(mockSSD, read(99))
		.Times(200)
		.WillRepeatedly(Return(val99));

	app.ExecuteCommand(commandVector);
}