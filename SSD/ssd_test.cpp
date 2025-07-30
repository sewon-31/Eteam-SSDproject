#include "gmock/gmock.h"
#include "ssd_command_parser.h"
#include "ssd.h"

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
		.WillRepeatedly(Return(vector<string>{ "ssd", "R", "0" }));

	app.run("ssd R 0");

	EXPECT_EQ("R", app.parsedCommand.at(1));
	EXPECT_EQ("0", app.parsedCommand.at(2));
}

TEST_F(SSDTestFixture, ReadTest) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "ssd", "R", "0" }));

	app.run("ssd R 0");

	EXPECT_EQ("0x00000000", app.getData(0));
}

TEST_F(SSDTestFixture, GetInvalidCommandTest) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, isValidCommand)
		.WillRepeatedly(Return(false));

	EXPECT_CALL(mockParser, getCommandVector)
		.Times(0);

	app.run("ssdr R 0");
}

TEST_F(SSDTestFixture, WriteText) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "ssd", "W", "0", "0x11112222"}));

	app.run("ssd W 0 0x11112222");

	EXPECT_EQ("0x11112222", app.getData(0));
}
