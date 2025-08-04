#include "gmock/gmock.h"
#include "ssd_command_builder.h"

using namespace testing;

class SSDCommandParserTestFixture : public Test {
public:
	SSDCommandBuilder builder;

	const string APP_NAME = "ssd";
	const string INVALID_APP_NAME = "ssdr";

	const string CMD_READ = "R";
	const string CMD_WRITE = "W";
	const string CMD_ERASE = "E";
	const string CMD_FLUSH = "F";
	const string CMD_INVALID = "D";

	const string VALID_LBA = "3";
	const string INVALID_LBA_OUT_OF_RANGE = "100";
	const string INVALID_LBA_NOT_A_NUMBER = "FF";

	const string MIN_SIZE = "0";
	const string MAX_SIZE = "10";
	const string INVALID_SIZE1 = "-1";
	const string INVALID_SIZE2 = "F";

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

TEST_F(SSDCommandParserTestFixture, TC_InvalidParameterCount1)
{
	builder.setCommandVector(vector<string>());
	EXPECT_FALSE(builder.isValidCommand());

	builder.setCommandVector({ CMD_READ, VALID_LBA, VALID_VALUE, VALID_VALUE });
	EXPECT_FALSE(builder.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, TC_InvalidOperationCommand)
{
	builder.setCommandVector({ CMD_INVALID, VALID_VALUE });
	EXPECT_FALSE(builder.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, TC_InvalidParameterCount2)
{
	builder.setCommandVector({ CMD_WRITE, VALID_LBA });
	EXPECT_FALSE(builder.isValidCommand());

	builder.setCommandVector({ CMD_READ, VALID_LBA, VALID_VALUE });
	EXPECT_FALSE(builder.isValidCommand());

	builder.setCommandVector({ CMD_ERASE, VALID_LBA });
	EXPECT_FALSE(builder.isValidCommand());

	builder.setCommandVector({ CMD_FLUSH, VALID_LBA });
	EXPECT_FALSE(builder.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, TC_InvalidLBA)
{
	builder.setCommandVector({ CMD_READ, INVALID_LBA_NOT_A_NUMBER });
	EXPECT_FALSE(builder.isValidCommand());

	builder.setCommandVector({ CMD_READ, INVALID_LBA_OUT_OF_RANGE });
	EXPECT_FALSE(builder.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, TC_InvalidValue)
{
	builder.setCommandVector({ CMD_WRITE, VALID_LBA, INVALID_VALUE1 });
	EXPECT_FALSE(builder.isValidCommand());

	builder.setCommandVector({ CMD_WRITE, VALID_LBA, INVALID_VALUE2 });
	EXPECT_FALSE(builder.isValidCommand());

	builder.setCommandVector({ CMD_WRITE, VALID_LBA, INVALID_VALUE3 });
	EXPECT_FALSE(builder.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, TC_InvalidSize)
{
	builder.setCommandVector({ CMD_ERASE, VALID_LBA, INVALID_SIZE1 });
	EXPECT_FALSE(builder.isValidCommand());

	builder.setCommandVector({ CMD_ERASE, VALID_LBA, INVALID_SIZE2 });
	EXPECT_FALSE(builder.isValidCommand());

	builder.setCommandVector({ CMD_ERASE, VALID_LBA, MIN_SIZE });
	EXPECT_TRUE(builder.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, TC_InvalidLBARange)
{
	builder.setCommandVector({ CMD_ERASE, "90", MAX_SIZE});
	EXPECT_TRUE(builder.isValidCommand());

	builder.setCommandVector({ CMD_ERASE, "89", MAX_SIZE});
	EXPECT_TRUE(builder.isValidCommand());

	builder.setCommandVector({ CMD_ERASE, "0", MIN_SIZE});
	EXPECT_TRUE(builder.isValidCommand());
}

TEST_F(SSDCommandParserTestFixture, TC_ValidValue)
{
	builder.setCommandVector({ CMD_WRITE, VALID_LBA, VALID_VALUE });
	EXPECT_TRUE(builder.isValidCommand());

	builder.setCommandVector({ CMD_READ, VALID_LBA });
	EXPECT_TRUE(builder.isValidCommand());

	builder.setCommandVector({ CMD_ERASE, VALID_LBA, MAX_SIZE });
	EXPECT_TRUE(builder.isValidCommand());
}