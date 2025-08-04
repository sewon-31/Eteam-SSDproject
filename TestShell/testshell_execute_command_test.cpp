#pragma once
#include "gmock/gmock.h"
#include "mock_ssd.h"
#include <string>

using namespace testing;
using namespace std;

class ShellCommandOperatorTestFixture : public Test {
public:
	TestShell app;

	void SetUp() override {
		app.setSSD(&mockSSD);
	}
public:
	MockSSDDriver mockSSD;
	string testData = "0x00012345";
};

TEST_F(ShellCommandOperatorTestFixture, Read) {
	vector<string> commandVector = { "read", "3" };

	EXPECT_CALL(mockSSD, read(_))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(ShellCommandOperatorTestFixture, Write) {
	vector<string> commandVector = { "write", "3", "0xAAAAAAAA"};

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(ShellCommandOperatorTestFixture, FullRead) {
	vector<string> commandVector = { "fullread" };

	EXPECT_CALL(mockSSD, read(_))
		.Times(100);

	app.ExecuteCommand(commandVector);
}
TEST_F(ShellCommandOperatorTestFixture, FullWrite) {
	vector<string> commandVector = { "fullwrite", "0xAAAAAAAA"};

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(100);

	app.ExecuteCommand(commandVector);
}
TEST_F(ShellCommandOperatorTestFixture, Erase) {
	vector<string> commandVector = { "erase", "5", "2"};

	EXPECT_CALL(mockSSD, erase(5, 2))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(ShellCommandOperatorTestFixture, EraseRange) {
	vector<string> commandVector = { "erase_range", "1", "4" };

	EXPECT_CALL(mockSSD, erase(1, 4))
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(ShellCommandOperatorTestFixture, Flush) {
	vector<string> commandVector = { "flush" };

	EXPECT_CALL(mockSSD, flush())
		.Times(1);

	app.ExecuteCommand(commandVector);
}
TEST_F(ShellCommandOperatorTestFixture, TestScript1) {
	vector<string> commandVector = { "1_" };

	EXPECT_CALL(mockSSD, write(_, _))
		.Times(100);
	EXPECT_CALL(mockSSD, read(_))
		.Times(100)
		.WillRepeatedly(Return(testData));

	app.ExecuteCommand(commandVector);
}
TEST_F(ShellCommandOperatorTestFixture, TestScript2) {
	vector<string> commandVector = { "2_" };

	EXPECT_CALL(mockSSD, write(_, _))
		.WillRepeatedly(Return());

	EXPECT_CALL(mockSSD, read(_))
		.WillRepeatedly(Return(testData));

	app.ExecuteCommand(commandVector);
}
TEST_F(ShellCommandOperatorTestFixture, TestScript3) {
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
TEST_F(ShellCommandOperatorTestFixture, TestScript4) {
	vector<string> commandVector = { "4_" };

	string writeValue = "0x00001111";
	string overWriteValue = "0x00009999";
	string erasedData = "0x00000000";

	const int LOOP = 30;
	const int TOTAL_SSD_CALL_IN_LOOP = 48;
	const int TOTAL_READ = 3;

	EXPECT_CALL(mockSSD, erase(_, 3))
		.Times(1 + TOTAL_SSD_CALL_IN_LOOP * LOOP);
	EXPECT_CALL(mockSSD, write(_, writeValue))
		.Times(TOTAL_SSD_CALL_IN_LOOP * LOOP);
	EXPECT_CALL(mockSSD, write(_, overWriteValue))
		.Times(TOTAL_SSD_CALL_IN_LOOP * LOOP);
	EXPECT_CALL(mockSSD, read(_))
		.Times(TOTAL_SSD_CALL_IN_LOOP * TOTAL_READ * LOOP)
		.WillRepeatedly(Return(erasedData));

	app.ExecuteCommand(commandVector);
}