#include "gmock/gmock.h"
#include "command_parser.h"

class CommandParserTestFixture : public ::testing::Test {
protected:
    CommandParser parser;
};

TEST_F(CommandParserTestFixture, ValidWriteCommandReturnsTrue) {
    parser.setCommand("write 3 0xABCDEF12");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, InvalidWriteCommandBadValueReturnsFalse) {
    parser.setCommand("write 3 0xXYZ123");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, InvalidWriteCommandLBAOutOfRangeReturnsFalse) {
    parser.setCommand("write 123 0xABCDEF12");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, InvalidWriteCommandMissingValue_ReturnsFalse) {
    parser.setCommand("write 3");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ValidReadCommandReturnsTrue) {
    parser.setCommand("read 0");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, InvalidReadCommandMissingArg_ReturnsFalse) {
    parser.setCommand("read");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, InvalidReadCommandTooManyArgs_ReturnsFalse) {
    parser.setCommand("read 3 extra");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ValidExitCommandReturnsTrue) {
    parser.setCommand("exit");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ValidHelpCommandReturnsTrue) {
    parser.setCommand("help");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ValidFullWriteCommandReturnsTrue) {
    parser.setCommand("fullwrite 0x1234ABCD");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, InvalidFullWriteCommandBadValueReturnsFalse) {
    parser.setCommand("fullwrite 0xGIBBERISH");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, UnknownCommandReturnsFalse) {
    parser.setCommand("foobar");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, EmptyInputReturnsFalse) {
    parser.setCommand("");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ExtraSpacesStillValid) {
    parser.setCommand("   write   3    0x1234ABCD   ");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ExtraSpacesTabStillValid) {
    parser.setCommand("   write   3                     0x1234ABCD   ");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ValidScript1ShortCommandReturnsTrue) {
    parser.setCommand("1_");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ValidScript1FullCommandReturnsTrue) {
    parser.setCommand("1_FullWriteAndReadCompare");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ValidScript2ShortCommandReturnsTrue) {
    parser.setCommand("2_");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ValidScript2FullCommandReturnsTrue) {
    parser.setCommand("2_PartialLBAWrite");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ValidScript3ShortCommandReturnsTrue) {
    parser.setCommand("3_");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, ValidScript3FullCommandReturnsTrue) {
    parser.setCommand("3_WriteReadAging");
    EXPECT_TRUE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, Script1CommandWithExtraArgsReturnsFalse) {
    parser.setCommand("1_ extraArg");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, Script2CommandWithExtraArgsReturnsFalse) {
    parser.setCommand("2_PartialLBAWrite unexpected");
    EXPECT_FALSE(parser.isValidCommand());
}

TEST_F(CommandParserTestFixture, Script3CommandWithExtraArgsReturnsFalse) {
    parser.setCommand("3_WriteReadAging junk1 junk2");
    EXPECT_FALSE(parser.isValidCommand());
}