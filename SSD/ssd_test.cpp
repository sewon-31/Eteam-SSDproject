#include "gmock/gmock.h"
#include "ssd_command_builder.h"
#include "ssd.h"
#include <random>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace testing;

class MockCommand : public ICommand {
public:
	MOCK_METHOD(void, run, (string& result), (override));
	MOCK_METHOD(void, execute, (string& result), (override));
	MOCK_METHOD(CmdType, getCmdType, (), (const, override));
	MOCK_METHOD(int, getLBA, (), (const, override));
};

class MockBuilder : public SSDCommandBuilder {
public:
	MOCK_METHOD(void, setCommandVector, (vector<string> commandVector), (override));
	MOCK_METHOD(bool, isValidCommand, (), (const, override));
	MOCK_METHOD(vector<string>, getCommandVector, (), (const, override));
	MOCK_METHOD(std::shared_ptr<ICommand>, createCommand, (std::vector<std::string>), (override));
};

class SSDTestFixture : public Test
{
protected:
	void SetUp() override {
		mockCmd = std::make_shared<MockCommand>();
		mockBuilder = std::make_shared<MockBuilder>();
	}

public:
	std::shared_ptr<MockCommand> mockCmd;
	std::shared_ptr<MockBuilder> mockBuilder;

	SSD app;

	FileInterface nandFile{ "../ssd_nand.txt" };
	FileInterface outputFile{ "../ssd_output.txt" };

	void processMockBuilderFunctions()
	{
		app.setBuilder(mockBuilder);
		EXPECT_CALL(*mockBuilder, isValidCommand)
			.WillRepeatedly(Return(true));
	}
};

TEST_F(SSDTestFixture, RunCommandTest1) {
	processMockBuilderFunctions();

	std::vector<std::string> input = { "R", "0" };

	EXPECT_CALL(*mockBuilder, createCommand(input))
		.WillOnce(Return(mockCmd));

	// if READ or FLUSH, directly call run
	EXPECT_CALL(*mockCmd, getCmdType)
		.WillOnce(Return(CmdType::READ));
		
	EXPECT_CALL(*mockCmd, run)
		.Times(1);

	app.run(input);
}

TEST_F(SSDTestFixture, RunCommandTest2) {
	processMockBuilderFunctions();

	std::vector<std::string> input = { "W", "0", "0x00001111"};

	EXPECT_CALL(*mockBuilder, createCommand(input))
		.WillOnce(Return(mockCmd));

	// if WRITE or ERASE, go to buffer
	EXPECT_CALL(*mockCmd, getCmdType)
		.WillOnce(Return(CmdType::WRITE));
		
	EXPECT_CALL(*mockCmd, run)
		.Times(0);

	app.run(input);
}

TEST_F(SSDTestFixture, GetInvalidCommandTest) {
	processMockBuilderFunctions();

	std::vector<std::string> input = { "R", "0", "0x00000000" };

	EXPECT_CALL(*mockBuilder, isValidCommand)
		.WillRepeatedly(Return(false));

	EXPECT_CALL(*mockBuilder, createCommand(input))
		.WillOnce(Return(nullptr));

	app.run(input);

	string expected;
	outputFile.fileOpen();
	outputFile.fileReadOneline(expected);
	outputFile.fileClose();

	EXPECT_EQ(expected, "ERROR");
}

TEST_F(SSDTestFixture, ReadTest) {
	app.run({ "R", "0" });
	
	// actual
	string actual;
	outputFile.fileOpen();
	outputFile.fileReadOneline(actual);
	outputFile.fileClose();

	// expected
	string expected;
	nandFile.fileOpen();
	if (nandFile.checkSize() >= 12)
		nandFile.fileReadOneline(expected);
	else
		expected = "0x00000000";
	nandFile.fileClose();

	// storage
	string storageData = app.getData(0);

	EXPECT_EQ(expected, storageData);
	EXPECT_EQ(actual, storageData);
}

// cannot run WriteText anymore
TEST_F(SSDTestFixture, WriteText) {
	vector<string> input = { "W", "0", "0x11112222" };

	app.run(input);

	// storage
	string storageData = app.getData(0);

	// expected
	string actual;
	nandFile.fileOpen();
	if (nandFile.checkSize() >= 12)
		nandFile.fileReadOneline(actual);
	else
		actual = "0x00000000";
	nandFile.fileClose();

	EXPECT_EQ("0x11112222", actual);
	EXPECT_EQ("0x11112222", storageData);

	app.clearData();
	EXPECT_EQ("0x00000000", app.getData(0));
}

/*
TEST_F(SSDTestFixture, TC_FULL_WRITE) {
	string str[100];
	char buffer[16];

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
	}

	EXPECT_EQ(1200, nandFile.checkSize());
}

TEST_F(SSDTestFixture, TC_FULL_WRITE_READ) {
	string str[100];
	char buffer[16];
	char ret = true;

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
	}

	EXPECT_EQ(1200, nandFile.checkSize());

	// to update storage from nand file
	app.run({ "R", "0" });

	for (int i = 0; i < 100; i++) {
		if (app.getData(i) != str[i]) {
			ret = false;
			break;
		}
	}
	EXPECT_TRUE(ret);
}

TEST_F(SSDTestFixture, TC_WRITE_OUTPUT) {
	char ret = true;
	std::string expected_str = "0x12341234";

	EXPECT_TRUE(app.updateOutputFile(expected_str));
	EXPECT_EQ(12, outputFile.checkSize());
}

TEST_F(SSDTestFixture, TC_RUN_WRITE) {
	app.run({ "W", "0", "0x11112222" });

	//FileInterface nandFile = { "../ssd_nand.txt" };

	string actual;
	nandFile.fileOpen();
	nandFile.fileReadOneline(actual);
	nandFile.fileClose();

	EXPECT_EQ(actual, "0x11112222");
}

TEST_F(SSDTestFixture, TC_RUN_READ) {
	app.run({ "R", "0" });

	//FileInterface nandFile = { "../ssd_nand.txt" };

	string expected;
	nandFile.fileOpen();
	nandFile.fileReadOneline(expected);
	nandFile.fileClose();

	//FileInterface outputFile = { "../ssd_output.txt" };

	string actual;
	outputFile.fileOpen();
	outputFile.fileReadOneline(actual);
	outputFile.fileClose();

	EXPECT_EQ(expected, actual);
}

TEST_F(SSDTestFixture, TC_RUN_ERASE) {
	string str[100];
	char buffer[16];
	char ret = true;

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
	}

	EXPECT_EQ(1200, nandFile.checkSize());

	// to update storage from nand file
	app.run({ "E", "0", "10" });

	app.run({ "R", "0" });
	EXPECT_EQ(app.getData(9), "0x00000000");
	EXPECT_EQ(app.getData(10), str[10]);
}
*/
