#include "gmock/gmock.h"
#include "ssd_command_parser.h"
#include "ssd.h"
#include <random>
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace testing;

class MockParser : public SSDCommandParser {
public:
	MOCK_METHOD(void, setCommand, (const string& command), (override));
	MOCK_METHOD(bool, isValidCommand, (), (const, override));
	MOCK_METHOD(vector<string>, getCommandVector, (), (const, override));
};

class SSDTestFixture : public Test
{
protected:
	void SetUp() override
	{
		app.setParser(&mockParser);
	}
public:
	MockParser mockParser;
	SSD app;
	FileInterface& nandFile = app.getNandFile();
	FileInterface& outputFile = app.getOutputFile();

	void processMockParserFunctions()
	{
		EXPECT_CALL(mockParser, setCommand)
			.Times(1);
		EXPECT_CALL(mockParser, isValidCommand)
			.WillRepeatedly(Return(true));
	}
};

TEST_F(SSDTestFixture, GetCommandTest) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "R", "0" }));

	app.run("R 0");

	EXPECT_EQ("R", app.parsedCommand.at(0));
	EXPECT_EQ("0", app.parsedCommand.at(1));
}

TEST_F(SSDTestFixture, ReadTest) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "R", "0" }));

	app.run("R 0");

	FileInterface nandFile = { "../ssd_nand.txt" };
	string expected;

	nandFile.fileOpen();
	if (nandFile.checkSize() >= 12)
		nandFile.fileReadOneline(expected);
	else
		expected = "0x00000000";
	nandFile.fileClose();

	EXPECT_EQ(expected, app.getData(0));
}

TEST_F(SSDTestFixture, GetInvalidCommandTest) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, isValidCommand)
		.WillRepeatedly(Return(false));

	EXPECT_CALL(mockParser, getCommandVector)
		.Times(0);

	app.run("R 0 0x00000000");
}

TEST_F(SSDTestFixture, WriteText) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "W", "0", "0x11112222"}));

	app.run("W 0 0x11112222");
	EXPECT_EQ("0x11112222", app.getData(0));

	app.clearData();
	EXPECT_EQ("0x00000000", app.getData(0));
}

TEST_F(SSDTestFixture, TC_FULL_WRITE) {
	string str[100];
	char buffer[16];

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		app.writeData(i, str[i]);
		//app.data[i] = str[i];
	}

	EXPECT_TRUE(app.writeNandFile());
	EXPECT_EQ(1200, nandFile.checkSize());
}

TEST_F(SSDTestFixture, TC_FULL_WRITE_READ) {
	string str[100];
	char buffer[16];
	char ret = true;

	for (int i = 0; i < 100; i++) {
		std::snprintf(buffer, sizeof(buffer), "0x%04X%04X", std::rand(), std::rand());
		str[i] = std::string(buffer);
		//app.data[i] = str[i];
		app.writeData(i, str[i]);
	}

	app.writeNandFile();
	EXPECT_EQ(1200, nandFile.checkSize());
	EXPECT_TRUE(app.readNandFile());

	for (int i = 0; i < 100; i++) {
		//if (app.data[i] != str[i]) {
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

	EXPECT_TRUE(app.writeOutputFile(expected_str));
	EXPECT_EQ(12, outputFile.checkSize());
}

TEST_F(SSDTestFixture, TC_RUN_WRITE) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "W", "0", "0x11112222"}));

	app.run("W 0 0x11112222");

	FileInterface nandFile = { "../ssd_nand.txt" };

	string actual;
	nandFile.fileOpen();
	nandFile.fileReadOneline(actual);
	nandFile.fileClose();

	EXPECT_EQ(actual, "0x11112222");
}

TEST_F(SSDTestFixture, TC_RUN_READ) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "R", "0" }));

	app.run("R 0");

	FileInterface nandFile = { "../ssd_nand.txt" };

	string expected;
	nandFile.fileOpen();
	nandFile.fileReadOneline(expected);
	nandFile.fileClose();

	FileInterface outputFile = { "../ssd_output.txt" };

	string actual;
	outputFile.fileOpen();
	outputFile.fileReadOneline(actual);
	outputFile.fileClose();

	EXPECT_EQ(expected, actual);
}