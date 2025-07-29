#include "gmock/gmock.h"
#include "SSDCommandParser.h"

using namespace testing;

class SSDCommandParserTestFixture : public Test {
public:
	SSDCommandParser parser;

	const string APP_NAME = "ssd";
	const string INVALID_APP_NAME = "ssdr";

	const string CMD_READ = "R";
	const string CMD_WRITE = "W";

	const string VALID_LBA = "3";
	const string INVALID_LBA_OUT_OF_RANGE = "100";
	const string INVALID_LBA_NOT_A_NUMBER = "FF";

	const string VALID_VALUE = "0xAAAABBBB";
	const string INVALID_VALUE = "0xG112BBBB";
};

TEST_F(SSDCommandParserTestFixture, InvalidAppName)
{
	string cmdStr = INVALID_APP_NAME + " " + CMD_READ + " " + VALID_VALUE;
	parser.setCommand(cmdStr);
	EXPECT_FALSE(parser.validateCommand());
}

TEST_F(SSDCommandParserTestFixture, InvalidParameterCount1)
{
	string cmdStr = APP_NAME;
	parser.setCommand(cmdStr);
	EXPECT_FALSE(parser.validateCommand());

	cmdStr = APP_NAME + " " + CMD_READ + " " + VALID_LBA + " " + VALID_VALUE + " " + VALID_VALUE;
	parser.setCommand(cmdStr);
	EXPECT_FALSE(parser.validateCommand());
}

TEST_F(SSDCommandParserTestFixture, InvalidOperationCommand)
{
	string cmdStr = APP_NAME + " E " + VALID_VALUE;
	parser.setCommand(cmdStr);
	EXPECT_FALSE(parser.validateCommand());
}

TEST_F(SSDCommandParserTestFixture, InvalidParameterCount2)
{
	string cmdStr = APP_NAME + " " + CMD_WRITE + " " + VALID_LBA;
	parser.setCommand(cmdStr);
	EXPECT_FALSE(parser.validateCommand());

	cmdStr = APP_NAME + " " + CMD_READ + " " + VALID_LBA + " " + VALID_VALUE;
	parser.setCommand(cmdStr);
	EXPECT_FALSE(parser.validateCommand());
}

TEST_F(SSDCommandParserTestFixture, InvalidLBA)
{
	string cmdStr = APP_NAME + " " + CMD_READ + " " + INVALID_LBA_OUT_OF_RANGE;
	parser.setCommand(cmdStr);
	EXPECT_FALSE(parser.validateCommand());

	cmdStr = APP_NAME + " " + CMD_READ + " " + INVALID_LBA_NOT_A_NUMBER;
	parser.setCommand(cmdStr);
	EXPECT_FALSE(parser.validateCommand());
}