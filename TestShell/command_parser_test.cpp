#include "gmock/gmock.h"
#include "command_parser.h"

class CommandParserTest : public ::testing::Test {
protected:
    CommandParser parser;
};

TEST_F(CommandParserTest, ValidWriteCommand_ReturnsTrue) {
    parser.setCommandVector("write 3 0xABCDEF12");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidWriteCommand_BadValue_ReturnsFalse) {
    parser.setCommandVector("write 3 0xXYZ123");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidWriteCommand_LBAOutOfRange_ReturnsFalse) {
    parser.setCommandVector("write 123 0xABCDEF12");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidWriteCommand_MissingValue_ReturnsFalse) {
    parser.setCommandVector("write 3");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidReadCommand_ReturnsTrue) {
    parser.setCommandVector("read 0");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidReadCommand_MissingArg_ReturnsFalse) {
    parser.setCommandVector("read");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidReadCommand_TooManyArgs_ReturnsFalse) {
    parser.setCommandVector("read 3 extra");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidExitCommand_ReturnsTrue) {
    parser.setCommandVector("exit");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidHelpCommand_ReturnsTrue) {
    parser.setCommandVector("help");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidFullWriteCommand_ReturnsTrue) {
    parser.setCommandVector("fullwrite 0x1234ABCD");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, InvalidFullWriteCommand_BadValue_ReturnsFalse) {
    parser.setCommandVector("fullwrite 0xGIBBERISH");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, UnknownCommand_ReturnsFalse) {
    parser.setCommandVector("foobar");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, EmptyInput_ReturnsFalse) {
    parser.setCommandVector("");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ExtraSpaces_StillValid) {
    parser.setCommandVector("   write   3    0x1234ABCD   ");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ExtraSpacesTab_StillValid) {
    parser.setCommandVector("   write   3                     0x1234ABCD   ");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidScript1ShortCommand_ReturnsTrue) {
    parser.setCommandVector("1_");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidScript1FullCommand_ReturnsTrue) {
    parser.setCommandVector("1_FullWriteAndReadCompare");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidScript2ShortCommand_ReturnsTrue) {
    parser.setCommandVector("2_");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidScript2FullCommand_ReturnsTrue) {
    parser.setCommandVector("2_PartialLBAWrite");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidScript3ShortCommand_ReturnsTrue) {
    parser.setCommandVector("3_");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, ValidScript3FullCommand_ReturnsTrue) {
    parser.setCommandVector("3_WriteReadAging");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTest, Script1CommandWithExtraArgs_ReturnsFalse) {
    parser.setCommandVector("1_ extraArg");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, Script2CommandWithExtraArgs_ReturnsFalse) {
    parser.setCommandVector("2_PartialLBAWrite unexpected");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTest, Script3CommandWithExtraArgs_ReturnsFalse) {
    parser.setCommandVector("3_WriteReadAging junk1 junk2");
    EXPECT_FALSE(parser.isValidCommand());
}