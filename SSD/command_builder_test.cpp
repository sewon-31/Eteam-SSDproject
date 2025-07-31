#include "gmock/gmock.h"
#include "ssd_command_builder.h"

using namespace testing;

class SSDCommandParserTestFixture : public Test {
public:
	SSDCommandBuilder parser;

	const string APP_NAME = "ssd";
	const string INVALID_APP_NAME = "ssdr";

	const string CMD_READ = "R";
	const string CMD_WRITE = "W";
	const string CMD_INVALID = "D";

	const string VALID_LBA = "3";
	const string INVALID_LBA_OUT_OF_RANGE = "100";
	const string INVALID_LBA_NOT_A_NUMBER = "FF";

	const string VALID_VALUE = "0xAAAABBBB";
	const string INVALID_VALUE1 = "0xG112BBBB";
	const string INVALID_VALUE2 = "0xAAA";
	const string INVALID_VALUE3 = "FFFFFFFFFF";

	string const createCommand(vector<string>& strs) {
		std::ostringstream oss;
		for (size_t i = 0; i < strs.size(); ++i) {
			oss << strs[i];
			if (i != strs.size() - 1)
				oss << " ";
		}

		return oss.str();
	}
};

TEST_F(SSDCommandParserTestFixture, InvalidParameterCount1)
{
	parser.setCommandVector(vector<string>());
	EXPECT_FALSE(parser.isValidCommand());

	parser.setCommandVector({ CMD_READ, VALID_LBA, VALID_VALUE, VALID_VALUE });
	EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, InvalidOperationCommand)
{
	parser.setCommandVector({ CMD_INVALID, VALID_VALUE });
	EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, InvalidParameterCount2)
{
	parser.setCommandVector({ CMD_WRITE, VALID_LBA });
	EXPECT_FALSE(parser.isValidCommand());

	parser.setCommandVector({ CMD_READ, VALID_LBA, VALID_VALUE });
	EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, InvalidLBA)
{
	parser.setCommandVector({ CMD_READ, INVALID_LBA_NOT_A_NUMBER });
	EXPECT_FALSE(parser.isValidCommand());

	parser.setCommandVector({ CMD_READ, INVALID_LBA_OUT_OF_RANGE });
	EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, InvalidValue)
{
	parser.setCommandVector({ CMD_WRITE, VALID_LBA, INVALID_VALUE1 });
	EXPECT_FALSE(parser.isValidCommand());

	parser.setCommandVector({ CMD_WRITE, VALID_LBA, INVALID_VALUE2 });
	EXPECT_FALSE(parser.isValidCommand());

	parser.setCommandVector({ CMD_WRITE, VALID_LBA, INVALID_VALUE3 });
	EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, ValidValue)
{
	parser.setCommandVector({ CMD_WRITE, VALID_LBA, VALID_VALUE });
	EXPECT_TRUE(parser.isValidCommand());

	parser.setCommandVector({ CMD_READ, VALID_LBA });
	EXPECT_TRUE(parser.isValidCommand());
}