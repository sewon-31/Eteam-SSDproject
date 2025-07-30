#include "gmock/gmock.h"
#include "ssd_command_parser.h"
#include "ssd.h"
#include <random>
#include <cstdlib>
#include <ctime>

using namespace testing;

#if 1
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
#endif

TEST_F(SSDTestFixture, GetCommandTest) {
	processMockParserFunctions();

	EXPECT_CALL(mockParser, getCommandVector())
		.WillRepeatedly(Return(vector<string>{ "ssd", "R", "0" }));

	app.run("ssd R 0");

	EXPECT_EQ("R", app.parsedCommand.at(1));
	EXPECT_EQ("0", app.parsedCommand.at(2));
}
#if 0
TEST(TC, TC_FULL_WRITE) {
	SSD temp;
	string str = "0x12341234";
	//char buffer[12];

#if 0
	for (int i = 0; i < 100; i++) {
		//std::snprintf(buffer, sizeof(buffer), "0x%08X", (std::rand() % INT_MAX + 1));
		//str[i] = std::string(buffer);
		temp.data[i] = str;
	}
#endif
	temp.writeNandFile();
}



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

	EXPECT_EQ("0x00000000", app.getData(0));
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
}
#endif