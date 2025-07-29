#include "gmock/gmock.h"
#include "SSDCommandParser.h"

using namespace testing;

TEST(SSDCommandParserTest, SettingCommand)
{
	SSDCommandParser parser;
	parser.setCommand("ssdr R 234");
	EXPECT_FALSE(parser.validateCommand());
}