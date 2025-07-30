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

TEST(SSDTest, GetCommandTest) {
	MockParser mockParser;

	EXPECT_CALL(mockParser, setCommand)
		.Times(1);

	EXPECT_CALL(mockParser, isValidCommand)
		.WillRepeatedly(Return(true));

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "ssd", "R", "0" }));

	SSD app;
	app.setParser(&mockParser);
	app.run("ssd R 0");

	EXPECT_EQ("R", app.parsedCommand.at(1));
	EXPECT_EQ("0", app.parsedCommand.at(2));
}