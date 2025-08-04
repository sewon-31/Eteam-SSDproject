#include "gmock/gmock.h"
#include "ssd_command_builder.h"
#include "ssd.h"
#include "file_interface.h"

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
	MOCK_METHOD(std::shared_ptr<ICommand>, createCommand, (vector<string>), (override));
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

	string nandFile = "../ssd_nand.txt";
	string outputFile = "../ssd_output.txt";

	void processMockBuilderFunctions()
	{
		app.setBuilder(mockBuilder);
		EXPECT_CALL(*mockBuilder, isValidCommand)
			.WillRepeatedly(Return(true));
	}
};

TEST_F(SSDTestFixture, TC_SsdRunCommandTest1) {
	processMockBuilderFunctions();
	vector<string> input = { "R", "0" };

	EXPECT_CALL(*mockBuilder, createCommand(input))
		.WillOnce(Return(mockCmd));

	// if READ or FLUSH, directly call run
	EXPECT_CALL(*mockCmd, getCmdType)
		.WillOnce(Return(CmdType::READ));
	EXPECT_CALL(*mockCmd, run)
		.Times(1);

	app.run(input);
	app.clearBufferAndDirectory();
}

// DISABLED - WriteCommand::getValue cannot be mocked
TEST_F(SSDTestFixture, TC_SsdDisabledRunCommandTest2) {
	processMockBuilderFunctions();
	vector<string> input = { "W", "0", "0x00001111" };

	EXPECT_CALL(*mockBuilder, createCommand(input))
		.WillOnce(Return(mockCmd));

	// if WRITE or ERASE, go to buffer
	EXPECT_CALL(*mockCmd, getCmdType)
		.WillRepeatedly(Return(CmdType::WRITE));
	EXPECT_CALL(*mockCmd, getLBA)
		.WillRepeatedly(Return(0));
	EXPECT_CALL(*mockCmd, run)
		.Times(0);

	app.run(input);
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_SsdGetInvalidCommandTest) {
	processMockBuilderFunctions();
	vector<string> input = { "R", "0", "0x00000000" };

	EXPECT_CALL(*mockBuilder, isValidCommand)
		.WillRepeatedly(Return(false));
	EXPECT_CALL(*mockBuilder, createCommand(input))
		.WillOnce(Return(nullptr));

	app.run(input);

	string expected;
	FileInterface::readLine(outputFile, expected);

	EXPECT_EQ(expected, "ERROR");
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_SsdReadTest) {
	app.run({ "R", "0" });

	string actual;
	FileInterface::readLine(outputFile, actual);

	string expected;
	if (FileInterface::getFileSize(nandFile) >= 12)
		FileInterface::readLine(nandFile, expected);
	else
		expected = "0x00000000";

	string storageData = app.getData(0);

	EXPECT_EQ(expected, storageData);
	EXPECT_EQ(actual, storageData);
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_SsdWriteText) {
	vector<string> input = { "W", "0", "0x11112222" };

	app.run(input);
	app.clearBuffer();

	string storageData = app.getData(0);

	string actual;
	if (FileInterface::getFileSize(nandFile) >= 12)
		FileInterface::readLine(nandFile, actual);
	else
		actual = "0x00000000";

	EXPECT_NE("0x11112222", actual);
	EXPECT_NE("0x11112222", storageData);

	app.run({ "R", "0" });
	app.clearBuffer();

	FileInterface::readLine(outputFile, actual);
	EXPECT_EQ("0x11112222", actual);

	app.clearData();
	EXPECT_EQ("0x00000000", app.getData(0));
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_FileFullWrite) {
	string str[100];
	char buffer[16];

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
		app.clearBuffer();
	}

	EXPECT_EQ(1200, FileInterface::getFileSize(nandFile));
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_FileFullWriteAndRead) {
	string str[100];
	char buffer[16];

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
		app.clearBuffer();
	}

	EXPECT_EQ(1200, FileInterface::getFileSize(nandFile));

	string actual;
	for (int i = 0; i < 100; i++) {
		app.run({ "R", std::to_string(i) });
		app.clearBuffer();
		FileInterface::readLine(outputFile, actual);
		EXPECT_EQ(actual, str[i]);
	}
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_FileWriteAndChecksize) {
	string expected_str = "0x12341234";

	EXPECT_TRUE(app.updateOutputFile(expected_str));
	EXPECT_EQ(12, FileInterface::getFileSize(outputFile));
}

TEST_F(SSDTestFixture, TC_FileErase) {
	string str[100];
	char buffer[16];

	FileInterface::clearFile(nandFile);

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
		app.clearBuffer();
	}

	EXPECT_EQ(1200, FileInterface::getFileSize(nandFile));

	app.run({ "E", "0", "10" });
	app.clearBuffer();

	string actual;

	app.run({ "R", "9" });
	app.clearBuffer();
	FileInterface::readLine(outputFile, actual);
	EXPECT_EQ(actual, "0x00000000");

	app.run({ "R", "10" });
	app.clearBuffer();
	FileInterface::readLine(outputFile, actual);
	EXPECT_EQ(actual, str[10]);

	app.clearBufferAndDirectory();
}
