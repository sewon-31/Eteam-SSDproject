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

	//void TearDown() override {
	//	app.clearBufferDirectory();
	//}

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
	app.clearBufferAndDirectory();
}

// DISABLED - WriteCommand::getValue cannot be mocked
TEST_F(SSDTestFixture, DISABLED_RunCommandTest2) {
	processMockBuilderFunctions();

	std::vector<std::string> input = { "W", "0", "0x00001111" };

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
	app.clearBufferAndDirectory();
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
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, WriteText) {
	vector<string> input = { "W", "0", "0x11112222" };

	app.run(input);
	app.clearBuffer();	// in real case, it would be re-created

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

	// buffer makes it false
	EXPECT_NE("0x11112222", actual);
	EXPECT_NE("0x11112222", storageData);

	app.run({ "R", "0" });
	app.clearBuffer();	// in real case, it would be re-created

	outputFile.fileOpen();
	outputFile.fileReadOneline(actual);
	outputFile.fileClose();

	EXPECT_EQ("0x11112222", actual);

	app.clearData();
	EXPECT_EQ("0x00000000", app.getData(0));
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_FULL_WRITE) {
	string str[100];
	char buffer[16];

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
		app.clearBuffer();	// in real case, it would be re-created
	}

	EXPECT_EQ(1200, nandFile.checkSize());
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_FULL_WRITE_READ) {
	string str[100];
	char buffer[16];
	char ret = true;

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
		app.clearBuffer();	// in real case, it would be re-created
	}

	EXPECT_EQ(1200, nandFile.checkSize());

	// to update storage from nand file
	//app.run({ "R", "0" });

	string actual;
	for (int i = 0; i < 100; i++) {

		app.run({ "R", std::to_string(i) });
		app.clearBuffer();	// in real case, it would be re-created

		outputFile.fileOpen();
		outputFile.fileReadOneline(actual);
		outputFile.fileClose();

		EXPECT_EQ(actual, str[i]);
		//if (app.getData(i) != str[i]) {
		//	ret = false;
		//	break;
		//}
	}
	//EXPECT_TRUE(ret);
	app.clearBufferAndDirectory();
}

TEST_F(SSDTestFixture, TC_WRITE_OUTPUT) {
	char ret = true;
	std::string expected_str = "0x12341234";

	EXPECT_TRUE(app.updateOutputFile(expected_str));
	EXPECT_EQ(12, outputFile.checkSize());
}

TEST_F(SSDTestFixture, TC_ERASE) {
	string str[100];
	char buffer[16];
	char ret = true;

	nandFile.fileClear();

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.run({ "W", std::to_string(i), str[i] });
		app.clearBuffer();	// in real case, it would be re-created
	}

	EXPECT_EQ(1200, nandFile.checkSize());

	app.run({ "E", "0", "10" });
	app.clearBuffer();	// in real case, it would be re-created

	string actual;

	app.run({ "R", "9" });
	app.clearBuffer();	// in real case, it would be re-created

	outputFile.fileOpen();
	outputFile.fileReadOneline(actual);
	outputFile.fileClose();

	EXPECT_EQ(actual, "0x00000000");

	app.run({ "R", "10" });
	app.clearBuffer();	// in real case, it would be re-created

	outputFile.fileOpen();
	outputFile.fileReadOneline(actual);
	outputFile.fileClose();
	EXPECT_EQ(actual, str[10]);

	app.clearBufferAndDirectory();
}