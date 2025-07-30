#include "gmock/gmock.h"
#include "command_parser.h"

class CommandParserTest : public ::testing::Test {
protected:
    CommandParser parser;
};

TEST_F(CommandParserTest, ValidWriteCommand_ReturnsTrue) {
    parser.setCommand("write 3 0xABCDEF12");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidWriteCommand_BadValue_ReturnsFalse) {
    parser.setCommand("write 3 0xXYZ123");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidWriteCommand_LBAOutOfRange_ReturnsFalse) {
    parser.setCommand("write 123 0xABCDEF12");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidWriteCommand_MissingValue_ReturnsFalse) {
    parser.setCommand("write 3");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidReadCommand_ReturnsTrue) {
    parser.setCommand("read 0");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidReadCommand_MissingArg_ReturnsFalse) {
    parser.setCommand("read");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidReadCommand_TooManyArgs_ReturnsFalse) {
    parser.setCommand("read 3 extra");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidExitCommand_ReturnsTrue) {
    parser.setCommand("exit");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidHelpCommand_ReturnsTrue) {
    parser.setCommand("help");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidFullWriteCommand_ReturnsTrue) {
    parser.setCommand("fullwrite 0x1234ABCD");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidFullWriteCommand_BadValue_ReturnsFalse) {
    parser.setCommand("fullwrite 0xGIBBERISH");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, UnknownCommand_ReturnsFalse) {
    parser.setCommand("foobar");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, EmptyInput_ReturnsFalse) {
    parser.setCommand("");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ExtraSpaces_StillValid) {
    parser.setCommand("   write   3    0x1234ABCD   ");
    EXPECT_TRUE(parser.isValidCommand());
}